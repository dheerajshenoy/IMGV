#include "ImageWidget.hpp"

ImageWidget::ImageWidget(QWidget *parent)
    : QGraphicsView(parent), m_zoomLevel(1.0)
{
    m_scene = new QGraphicsScene(this);
    setScene(m_scene);

    m_pixmapItem = new QGraphicsPixmapItem();
    m_scene->addItem(m_pixmapItem);

    // no antialiasing or filtering, we want to see the exact image content
    setRenderHint(QPainter::Antialiasing, false);
    setDragMode(QGraphicsView::ScrollHandDrag);
    setOptimizationFlags(QGraphicsView::DontSavePainterState);
    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse); // zoom at cursor position
    /*setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);*/
    /*setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);*/
    setInteractive(true);
    setMouseTracking(true);
}

void ImageWidget::setPixmap(const QString &filePath)
{
    QPixmap pixmap(filePath);
    if (pixmap.isNull()) {
        qWarning("Failed to load image from %s", qPrintable(filePath));
        return;
    }

    m_pixmapItem->setPixmap(pixmap);
    m_pixmapItem->setTransformationMode(Qt::SmoothTransformation);

    // Fit the view to the new pixmap
    /*fitInView(m_pixmapItem, Qt::KeepAspectRatio);*/
}

void ImageWidget::zoomOriginal() {
    m_zoomLevel = 0;
    m_fit = false;
    setMatrix();
}

void ImageWidget::zoomIn(int level) {
    qDebug() << "DD";
    m_zoomLevel += level;
    m_fit = false;
    setMatrix();
}

void ImageWidget::zoomOut(int level) {
    m_zoomLevel -= level;
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
    mat.scale(newScale, newScale);
    /*mat.rotateRadians(rotationRadians());*/

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
    m_pixmapItem->setPixmap(QPixmap(file));
    this->scene()->setSceneRect(m_pixmapItem->boundingRect());
}

void ImageWidget::wheelEvent(QWheelEvent *e)
{
    if (e->angleDelta().y() > 0)
        zoomIn(3);
    else
        zoomOut(3);

    /*QGraphicsView::wheelEvent(e);*/
}
