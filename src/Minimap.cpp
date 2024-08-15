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

void Minimap::setSize(const QSize size) noexcept
{
    m_pix_scale_size = size;
}

void Minimap::setPixmap(const QPixmap &pix) noexcept
{
    m_pix = pix.scaled(m_pix_scale_size, Qt::KeepAspectRatio);
    m_pixitem->setPixmap(m_pix);
    m_scene->setSceneRect(m_pixitem->boundingRect());
    /*fitInView(m_pixitem->boundingRect(), Qt::KeepAspectRatio);*/
    m_mainPixmapBoundingRect = pix.rect();
}

void Minimap::updateRect(const QRectF rect) noexcept
{
    qreal scale = m_pix.width() / m_mainPixmapBoundingRect.width();
    QRectF scaledRect(
        rect.x() * scale,
        rect.y() * scale,
        rect.width() * scale,
        rect.height() * scale
    );

    if (scaledRect.width() < m_pix.width())
    {
        if (m_auto_hide)
            this->setVisible(true);
        if (!m_rectitem->isVisible())
            m_rectitem->setVisible(true);
        m_rectitem->setRect(scaledRect);
    }
    else if (m_auto_hide)
        this->setVisible(false);
    else
        m_rectitem->setVisible(false);
}

void Minimap::setRectColor(const QString color) noexcept
{
    m_rectitem->setColor(color);
}

void Minimap::setRectFillColor(const QString color) noexcept
{
    m_rectitem->setFill(color);
}

void Minimap::setLocation(const Location loc) noexcept
{
    m_location = loc;
}

void Minimap::setRectAlpha(const float alpha) noexcept
{
    m_rectitem->setAlpha(alpha);
}

void Minimap::setAutoHide(const bool state) noexcept
{
    m_auto_hide = state;
}
