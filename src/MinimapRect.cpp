#include "MinimapRect.hpp"

MinimapRect::MinimapRect(QGraphicsItem *parent)
    : QGraphicsRectItem(parent)
{
    this->setPen(QPen(QColor(Qt::black)));
}

