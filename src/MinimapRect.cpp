#include "MinimapRect.hpp"

MinimapRect::MinimapRect(QGraphicsItem *parent)
    : QGraphicsRectItem(parent)
{
    this->setPen(QPen(QColor(Qt::black)));
}

void MinimapRect::setColor(const QString color) noexcept
{
    if (QColor(color).isValid())
        this->setPen(QPen(QColor::fromString(color)));
}

void MinimapRect::setFill(const QString color) noexcept
{
    if (QColor(color).isValid())
    {
        m_fill_color = QColor::fromString(color);
        this->setBrush(QBrush(m_fill_color));
    }

}

void MinimapRect::setAlpha(const float alpha) noexcept
{
    this->setOpacity(alpha);
}
