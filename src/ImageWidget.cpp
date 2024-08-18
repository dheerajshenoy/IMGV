#include "ImageWidget.hpp"

ImageWidget::ImageWidget(QWidget *parent)
    : QGraphicsView(parent), m_zoomLevel(0.0)
{
    m_scene = new QGraphicsScene(this);
    setScene(m_scene);

    // For normal images
    m_pixmapItem = new QGraphicsPixmapItem;

    QPointF bottomLeftInView = this->viewport()->rect().bottomLeft();
    QPointF bottomLeftCornerInScene = this->mapToScene(bottomLeftInView.toPoint());
    // For animated gifs
    m_movieItem = new MovieItem;

    m_scene->addItem(m_pixmapItem);
    m_scene->addItem(m_movieItem);

    m_movie = new QMovie;
    connect(m_movie, &QMovie::frameChanged, this, &ImageWidget::GifLoopHandler);
    m_movieItem->setMovie(m_movie);
    /*m_movie->setCacheMode(QMovie::CacheMode::CacheAll);*/
    /*m_pixmapItem->setCacheMode(QGraphicsPixmapItem::CacheMode::ItemCoordinateCache);*/
    /*m_movieItem->setCacheMode(QGraphicsPixmapItem::CacheMode::ItemCoordinateCache);*/

    setRenderHint(QPainter::Antialiasing, false);
    setDragMode(QGraphicsView::ScrollHandDrag);
    setOptimizationFlags(QGraphicsView::DontSavePainterState);
    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse); // zoom at cursor position
    /*setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);*/
    /*setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);*/
    setInteractive(true);
    setMouseTracking(true);
    setAcceptDrops(true);

    m_minimap->raise();
    m_minimap->setVisible(false);

    m_pix_analyser = new PixAnalyser(this);
    m_pix_analyser->raise();


    connect(m_movieItem, &MovieItem::frameChanged, this, [&]() {
        if (m_pix_analyse_mode && !m_pixmapItem->isVisible())
            m_pix_analyser->setPixmap(m_movieItem->currentPixmap());
    });

    connect(m_pix_analyser, &PixAnalyser::visibilityChanged, this, [&](bool state) {
        emit pixAnalyserVisibilityChanged(state);
        setPixAnalyseMode(state);
    });

    connect(m_minimap, &Minimap::visibilityChanged, this, [&](bool state) {
        emit minimapVisibilityChanged(state);
    });

    connect(m_pix_analyser, &PixAnalyser::pickColor, this, [&]() {
        /*emit pix*/
        setPixAnalyseMode(true);
    });

    switch (m_minimap->location())
    {

        case Minimap::Location::BottomRight:
            m_minimap->move(viewport()->width() - m_minimap->width() - 30,
                            viewport()->height() - m_minimap->height() - 30);
            break;

        case Minimap::Location::BottomLeft:
            m_minimap->move(10, viewport()->height() - m_minimap->height() - 10);
            break;

        case Minimap::Location::TopLeft:
            m_minimap->move(10, 10);
            break;

        case Minimap::Location::TopRight:
            m_minimap->move(viewport()->width() - m_minimap->width(), 10);
            break;

        case Minimap::Location::Custom:
            // TODO: Custom Minimap Location
            break;
    }

}

void ImageWidget::zoomOriginal() noexcept
{
    m_zoomLevel = 0;
    m_fit = false;
    setMatrix();
    emit zoomChanged(m_zoomLevel);

    /*emit getRegion(visibleRect(), m_scene->sceneRect());*/
    if (m_minimap_mode)
        emit getRegion(mapToScene(viewport()->rect()).boundingRect());
}

void ImageWidget::zoomIn() noexcept
{
    if (m_zoomLevel + m_zoomFactor >= 200) return;
    m_zoomLevel += m_zoomFactor;
    m_fit = false;
    setMatrix();
    emit zoomChanged(m_zoomLevel);
    if (m_minimap_mode)
        emit getRegion(mapToScene(viewport()->rect()).boundingRect());
}

void ImageWidget::zoomOut() noexcept
{
    if (m_zoomLevel - m_zoomFactor < -50) return;
    m_zoomLevel -= m_zoomFactor;
    m_fit = false;
    setMatrix();
    emit zoomChanged(m_zoomLevel);
    if (m_minimap_mode)
        emit getRegion(mapToScene(viewport()->rect()).boundingRect());
}

void ImageWidget::updateView() noexcept
{
    setTransform(QTransform::fromScale(m_zoomLevel, m_zoomLevel));
    fitInView(m_pixmapItem, Qt::KeepAspectRatio);  // Ensure the item is centered and scaled properly
    fitInView(m_movieItem, Qt::KeepAspectRatio);  // Ensure the item is centered and scaled properly
}

qreal ImageWidget::scale() const noexcept
{
    auto square = [](qreal value) { return value * value; };
    return std::sqrt(square(this->transform().m11()) + square(this->transform().m12()));
}

void ImageWidget::setMatrix() noexcept
{
    qreal newScale = std::pow(2.0, m_zoomLevel / 10.0);

    QTransform mat;

    mat.rotateRadians(qDegreesToRadians(m_rotate));

    if (!m_horizontal_flip && !m_vertical_flip)
        mat.scale(newScale, newScale);
    else
    {
        if (m_horizontal_flip)
            mat.scale(-1 * newScale, newScale);
        if (m_vertical_flip)
            mat.scale(newScale, -1 * newScale);
    }

    this->setTransform(mat);
    /*emit zoomChanged(scale());*/
}

void ImageWidget::zoomFit() noexcept
{
    /* Fit in view by KeepAspectRatioByExpanding does not keep the position
     * find out the current viewport center move back to that position after
     * fitting. It is done here instead of inside the resize event handler
     * because fitInView may be triggered from a number of events, not just
     * the resize event.
     */
    auto cr = QRect(this->viewport()->rect().center(), QSize(2, 2));
    auto cen = this->mapToScene(cr).boundingRect().center();
    this->fitInView(m_pixmapItem, m_aspect_ratio_mode);
    this->fitInView(m_movieItem, m_aspect_ratio_mode);

    m_zoomLevel = int(10.0 * std::log2(scale()));
    emit zoomChanged(m_zoomLevel);

    if (m_minimap_mode)
        emit getRegion(mapToScene(viewport()->rect()).boundingRect());
    m_fit = true;

    if (m_aspect_ratio_mode == Qt::KeepAspectRatioByExpanding)
        this->centerOn(cen);

    /*emit zoomChanged(scale());*/
}

void ImageWidget::loadFile(QString&& file) noexcept
{
    /*m_rotate = 0.0f;*/
    /*m_zoomLevel = 0.0f;*/
    /*setMatrix();*/
    resetScrollbars();
    int w = 0, h = 0;
    QImageReader imreader(file);
    if (imreader.supportsAnimation())
    {
        m_pixmapItem->hide();
        m_movieItem->show();
        m_movie->stop();
        m_movie->setFileName(file);
        w = m_movie->currentPixmap().width();
        h = m_movie->currentPixmap().height();
        m_movie->start();
        m_scene->setSceneRect(m_movieItem->boundingRect());
        if (m_pix_analyse_mode)
            m_pix_analyser->setPixmap(m_movieItem->currentPixmap());
    }
    else
    {
        m_movieItem->hide();
        m_pixmapItem->show();
        QPixmap pix;
        if (!QPixmapCache::find(file, &pix))
        {
            pix = utils::decodeToPixmap(file);
            QPixmapCache::insert(file, pix);
        }
        w = pix.width(); h = pix.height();
        m_pixmapItem->setPixmap(pix);
        m_scene->setSceneRect(m_pixmapItem->boundingRect());
        if (m_pix_analyse_mode)
            m_pix_analyser->setPixmap(pix);
    }
    emit fileLoaded(file);
    emit fileDim(w, h);

    if (m_fit_image_on_load)
    {
        switch(m_fit_image_on_load_mode)
        {
            case FitOnLoad::FitToWidth:
                fitToWidth();
            break;

            case FitOnLoad::FitToHeight:
                fitToHeight();
            break;

            default:
                break;
        }
    }
}

void ImageWidget::loadFile(const QString& file) noexcept
{
    /*m_rotate = 0.0f;*/
    /*m_zoomLevel = 0.0f;*/
    /*setMatrix();*/
    resetScrollbars();
    int w = 0, h = 0;
    QImageReader imreader(file);
    if (imreader.supportsAnimation())
    {
        m_pixmapItem->hide();
        m_movieItem->show();
        m_movie->stop();
        m_movie->setFileName(file);
        w = m_movie->currentPixmap().width();
        h = m_movie->currentPixmap().height();
        m_movie->start();
        m_scene->setSceneRect(m_movieItem->boundingRect());
        if (m_pix_analyse_mode)
            m_pix_analyser->setPixmap(m_movieItem->currentPixmap());
    }
    else
    {
        m_movieItem->hide();
        m_pixmapItem->show();
        QPixmap pix;

        if (!QPixmapCache::find(file, &pix))
        {
            auto format = utils::detectImageFormat(file);
            qDebug() << format;
            pix = utils::decodeToPixmap(file);

            QPixmapCache::insert(file, pix);
        }
        w = pix.width(); h = pix.height();
        m_pixmapItem->setPixmap(pix);
        m_scene->setSceneRect(m_pixmapItem->boundingRect());
        if (m_pix_analyse_mode)
            m_pix_analyser->setPixmap(pix);
    }
    emit fileLoaded(file);
    emit fileDim(w, h);

    if (m_fit_image_on_load)
    {
        switch(m_fit_image_on_load_mode)
        {
            case FitOnLoad::FitToWidth:
                fitToWidth();
            break;

            case FitOnLoad::FitToHeight:
                fitToHeight();
            break;

            default:
                break;
        }
    }
}


void ImageWidget::wheelEvent(QWheelEvent *e) noexcept
{
    if (e->angleDelta().y() > 0)
        zoomIn();
    else
        zoomOut();

    /*QGraphicsView::wheelEvent(e);*/
}

void ImageWidget::dragEnterEvent(QDragEnterEvent *e) noexcept
{
    const QMimeData *mimedata = e->mimeData();
    if (mimedata->hasUrls())
        e->acceptProposedAction();

}

void ImageWidget::dragLeaveEvent(QDragLeaveEvent *e) noexcept
{
    e->accept();
}

void ImageWidget::dragMoveEvent(QDragMoveEvent *e) noexcept
{
    e->accept();
    e->acceptProposedAction();
}

void ImageWidget::dropEvent(QDropEvent *e) noexcept
{
    /*if (e->source() == this) return;*/
    if (e->mimeData()->hasUrls())
    {
        QList<QUrl> urllist = e->mimeData()->urls();
        foreach(QUrl url, urllist)
        {
            if (url.isLocalFile())
            {
                QString filepath = url.toLocalFile();
                loadFile(filepath);
                emit fileLoaded(filepath);
                emit droppedImage(filepath);
            }
        }
    }
    e->acceptProposedAction();
}

void ImageWidget::rotate(const qreal& degrees) noexcept
{
    m_rotate += degrees;
    setMatrix();
}

void ImageWidget::rotate(qreal&& degrees) noexcept
{
    m_rotate += degrees;
    setMatrix();
}

void ImageWidget::flipVertical() noexcept
{
    m_vertical_flip = !m_vertical_flip;
    setMatrix();
}

void ImageWidget::flipHorizontal() noexcept
{
    m_horizontal_flip = !m_horizontal_flip;
    setMatrix();
}

void ImageWidget::fitToWidth() noexcept
{
    m_aspect_ratio_mode = Qt::IgnoreAspectRatio;
    zoomFit();
}

void ImageWidget::fitToHeight() noexcept
{
    m_aspect_ratio_mode = Qt::KeepAspectRatio;
    zoomFit();
}

void ImageWidget::GifLoopHandler(const int& frameNumber) noexcept
{
    static int loopCount = 0;
    if(frameNumber == (m_movie->frameCount()-1)) {
        loopCount++;
        if(loopCount >= m_gif_max_loop_count)
            m_movie->stop();
    }
}

void ImageWidget::resetRotation() noexcept
{
    m_rotate = 0;
    setMatrix();
}

void ImageWidget::setScrollBarsVisibility(const bool& state) noexcept
{
    if (state)
    {
        this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    }
    else {
        this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }
}

void ImageWidget::setScrollBarsVisibility(bool&& state) noexcept
{
    if (state)
    {
        this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    }
    else {
        this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }
}

void ImageWidget::closeFile() const noexcept
{
    m_pixmapItem->hide();
    m_movieItem->hide();
}

void ImageWidget::moveLeft() noexcept
{
    auto scrollbar = this->horizontalScrollBar();
    scrollbar->setValue(scrollbar->value() - m_horizontal_scroll_factor);
}


void ImageWidget::moveDown() noexcept
{
    auto scrollbar = this->verticalScrollBar();
    scrollbar->setValue(scrollbar->value() + m_horizontal_scroll_factor);
}


void ImageWidget::moveUp() noexcept
{
    auto scrollbar = this->verticalScrollBar();
    scrollbar->setValue(scrollbar->value() - m_horizontal_scroll_factor);
}

void ImageWidget::moveRight() noexcept
{
    auto scrollbar = this->horizontalScrollBar();
    scrollbar->setValue(scrollbar->value() + m_vertical_scroll_factor);
}

void ImageWidget::resetScrollbars() const noexcept
{
    this->horizontalScrollBar()->setValue(0);
    this->verticalScrollBar()->setValue(0);
}

void ImageWidget::mouseMoveEvent(QMouseEvent *e) noexcept
{
    QGraphicsView::mouseMoveEvent(e);
    if (m_pix_analyse_mode)
    {
        /*emit mouseMoved(mapToScene(e->pos()));*/
        m_pix_analyser->analysePix(mapToScene(e->pos()));
        this->viewport()->setCursor(Qt::CrossCursor);
    }

    if (m_minimap_mode && m_panning)
        emit getRegion(mapToScene(viewport()->rect()).boundingRect());
}

void ImageWidget::mousePressEvent(QMouseEvent *e) noexcept
{
    if (m_pix_analyse_mode)
    {
        setPixAnalyseMode(false, false);
        m_pix_analyser->setColorPicked(true);
    }
    m_panning = true;
    QGraphicsView::mousePressEvent(e);
}

void ImageWidget::mouseReleaseEvent(QMouseEvent *e) noexcept
{
    m_panning = false;
    QGraphicsView::mouseReleaseEvent(e);
}

void ImageWidget::resizeEvent(QResizeEvent *e) noexcept
{
    switch (m_minimap->location())
    {

        case Minimap::Location::BottomRight:
            m_minimap->move(viewport()->width() - m_minimap->width() - 30,
                            viewport()->height() - m_minimap->height() - 30);
            break;

        case Minimap::Location::BottomLeft:
            m_minimap->move(10, viewport()->height() - m_minimap->height() - 10);
            break;

        case Minimap::Location::TopLeft:
            m_minimap->move(10, 10);
            break;

        case Minimap::Location::TopRight:
            m_minimap->move(viewport()->width() - m_minimap->width(), 10);
            break;

        case Minimap::Location::Custom:
            // TODO: Custom Minimap Location
            break;
    }
    QGraphicsView::resizeEvent(e);
}

const QRectF ImageWidget::visibleRect() const noexcept
{
    const QRect viewportRect(QPoint(horizontalScrollBar()->value(), verticalScrollBar()->value()), viewport()->size());
    const auto mat = transform().inverted();
    return mat.mapRect(viewportRect);
}

void ImageWidget::setMinimapMode(const bool& state) noexcept
{
    m_minimap_mode = state;

    if (m_minimap_mode)
    {
        m_minimap->setVisibleIfNotAutoHideMode(true);

        m_minimap->setPixmap(m_pixmapItem->pixmap());
        connect(this, &ImageWidget::getRegion, m_minimap, &Minimap::updateRect);
    }
    else
    {
        m_minimap->setVisible(false);
        disconnect(this, &ImageWidget::getRegion, m_minimap, &Minimap::updateRect);
    }
}

void ImageWidget::setPixAnalyseMode(const bool& state, const bool&& closeDialog) noexcept
{
    m_pix_analyse_mode = state;
    if (state)
    {
        m_pix_analyser->show();
        if (m_pixmapItem->isVisible())
            m_pix_analyser->setPixmap(m_pixmapItem->pixmap());
        else
            m_pix_analyser->setPixmap(m_movieItem->currentPixmap());
        this->viewport()->setCursor(Qt::CrossCursor);
    }
    else
    {
        if (closeDialog)
            m_pix_analyser->close();
        this->viewport()->setCursor(Qt::OpenHandCursor);
    }
}

ImageWidget::~ImageWidget()
{
    delete m_pix_analyser;
    m_pix_analyser = nullptr;
}
