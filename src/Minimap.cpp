#include "Minimap.hpp"

Minimap::Minimap(QWidget *parent)
    : QWidget(parent)
{
    m_gv->setScene(m_scene);
    m_scene->addItem(m_pixitem);
    m_scene->addItem(m_rectitem);
    m_layout->addWidget(m_gv);
    this->setLayout(m_layout);
    this->show();

    m_gv->setInteractive(true);
}

void Minimap::setRectSize(const QSize size) noexcept
{
    m_pix_scale_size = size;
}

void Minimap::setPixmap(const QPixmap &pix) noexcept
{
    m_pix = pix.scaled(m_pix_scale_size, Qt::KeepAspectRatio);
    m_pixitem->setPixmap(m_pix);
}

void Minimap::setMainPixmapBoundingRect(const QRectF &rect) noexcept
{
    m_mainPixmapBoundingRect = rect;
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

    if (scaledRect.width() > m_pix.width())
        m_rectitem->setVisible(false);
    else
    {
        if (!m_rectitem->isVisible())
            m_rectitem->setVisible(true);
        m_rectitem->setRect(scaledRect);
    }
}


void Minimap::setRectColor(const QString color) noexcept
{
    if (QColor(color).isValid())
        m_rectitem->setColor(color);
}
