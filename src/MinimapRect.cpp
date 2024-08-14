#include "MinimapRect.hpp"

MinimapRect::MinimapRect(QGraphicsItem *parent)
    : QGraphicsRectItem(parent)
{
    this->setPen(QPen(QColor(Qt::black)));
}

void MinimapRect::setColor(const QString color) noexcept
{
    this->setPen(QPen(QColor::fromString(color)));
}
