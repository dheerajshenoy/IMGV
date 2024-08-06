#include "ImageWidget.hpp"

ImageWidget::ImageWidget(QWidget *parent)
    : QGraphicsView(parent), m_zoomLevel(1.0)
{
    m_scene = new QGraphicsScene(this);
    setScene(m_scene);

    m_pixmapItem = new QGraphicsPixmapItem();
    m_scene->addItem(m_pixmapItem);

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

}

void ImageWidget::setPixmap(const QString &filePath)
{
    QPixmap pixmap(filePath);

    m_pixmapItem->setPixmap(pixmap);
    m_pixmapItem->setTransformationMode(Qt::SmoothTransformation);

    // Fit the view to the new pixmap
    fitInView(m_pixmapItem, Qt::KeepAspectRatio);
}

void ImageWidget::zoomOriginal() {
    m_zoomLevel = 0;
    m_fit = false;
    setMatrix();
}

void ImageWidget::zoomIn() {
    m_zoomLevel += m_zoomFactor;
    m_fit = false;
    setMatrix();
}

void ImageWidget::zoomOut() {
    m_zoomLevel -= m_zoomFactor;
    m_fit = false;
    setMatrix();
}

void ImageWidget::updateView()
{
    setTransform(QTransform::fromScale(m_zoomLevel, m_zoomLevel));
    fitInView(m_pixmapItem, Qt::KeepAspectRatio);  // Ensure the item is centered and scaled properly
}

qreal ImageWidget::scale() const {
    auto square = [](qreal value) { return value * value; };
    return std::sqrt(square(this->transform().m11()) + square(this->transform().m12()));
}

void ImageWidget::setMatrix() {
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

void ImageWidget::zoomFit() {
    /* Fit in view by KeepAspectRatioByExpanding does not keep the position
     * find out the current viewport center move back to that position after
     * fitting. It is done here instead of inside the resize event handler
     * because fitInView may be triggered from a number of events, not just
     * the resize event.
     */
    auto cr = QRect(this->viewport()->rect().center(), QSize(2, 2));
    auto cen = this->mapToScene(cr).boundingRect().center();
    this->fitInView(m_pixmapItem, m_aspect_ratio_mode);

    m_zoomLevel = int(10.0 * std::log2(scale()));
    m_fit = true;

    if (m_aspect_ratio_mode == Qt::KeepAspectRatioByExpanding)
        this->centerOn(cen);

    /*emit zoomChanged(scale());*/
}

void ImageWidget::loadFile(QString file)
{
    m_rotate = 0.0f;
    m_zoomLevel = 0.0f;
    setMatrix();
    m_pixmapItem->setPixmap(QPixmap(file));
    m_scene->setSceneRect(m_pixmapItem->boundingRect());
    emit fileLoaded(file);
}

void ImageWidget::wheelEvent(QWheelEvent *e)
{
    if (e->angleDelta().y() > 0)
        zoomIn();
    else
        zoomOut();

    /*QGraphicsView::wheelEvent(e);*/
}

void ImageWidget::dragEnterEvent(QDragEnterEvent *e)
{
    const QMimeData *mimedata = e->mimeData();

    /*if (mimedata->hasFormat("image/jpeg") ||*/
    /*    mimedata->hasFormat("image/png") ||*/
    /*    mimedata->hasFormat("image/bmp"))*/
    if (mimedata->hasUrls())
        e->acceptProposedAction();

}

void ImageWidget::dragLeaveEvent(QDragLeaveEvent *e)
{
    e->accept();
}

void ImageWidget::dragMoveEvent(QDragMoveEvent *e)
{
    e->accept();
    e->acceptProposedAction();
}

void ImageWidget::dropEvent(QDropEvent *e)
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
            }
        }
    }
    e->acceptProposedAction();
}

void ImageWidget::rotate(qreal degrees)
{
    m_rotate += degrees;
    setMatrix();
}

void ImageWidget::rotateAnticlockwise()
{
    rotate(-90);
}

void ImageWidget::rotateClockwise()
{
    rotate(90);
}

void ImageWidget::flipVertical()
{
    m_vertical_flip = !m_vertical_flip;
    setMatrix();
}

void ImageWidget::flipHorizontal()
{
    m_horizontal_flip = !m_horizontal_flip;
    setMatrix();
}

void ImageWidget::fitToWidth()
{
    m_aspect_ratio_mode = Qt::IgnoreAspectRatio;
    zoomFit();
}

void ImageWidget::fitToHeight()
{
    m_aspect_ratio_mode = Qt::KeepAspectRatio;
    zoomFit();
}
