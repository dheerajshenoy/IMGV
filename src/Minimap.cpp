#include "Minimap.hpp"

Minimap::Minimap(QWidget *parent)
    : QGraphicsView(parent)
{
    setScene(m_scene);
    m_scene->addItem(m_pixitem);
    m_scene->addItem(m_rectitem);
    setInteractive(true);
    /*setFrameStyle);*/
}

void Minimap::setPixmap(const QPixmap &pix) noexcept
{
    m_pix = pix.scaled(m_pix_scale_size, Qt::KeepAspectRatio);
        m_pixitem->setPixmap(m_pix);
    m_scene->setSceneRect(m_pixitem->boundingRect());
    /*fitInView(m_pixitem->boundingRect(), Qt::KeepAspectRatio);*/
    m_mainPixmapBoundingRect = pix.rect();
}

void Minimap::updateRect(QRectF rect) noexcept
{
    qreal scale = m_pix.width() / m_mainPixmapBoundingRect.width();
    QRectF scaledRect(
        rect.x() * scale,
        rect.y() * scale,
        rect.width() * scale,
        rect.height() * scale
    );

    // If user is viewing the image zoomed into a region, then show the minimap if auto hide is enabled and hide it when the viewing region is the complete image.
    if (scaledRect.width() < m_pix.width())
    {
        if (m_auto_hide)
            this->setVisible(true);
        if (!m_rectitem->isVisible())
            m_rectitem->setVisible(true);
        m_rectitem->setRect(scaledRect);
    }
    else
    {
        if (m_auto_hide)
            this->setVisible(false);
        else
            m_rectitem->setVisible(false);
    }
}
