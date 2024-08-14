#include "MinimapRect.hpp"

MinimapRect::MinimapRect(QGraphicsItem *parent)
    : QGraphicsRectItem(parent)
{

}

void MinimapRect::mousePressEvent(QGraphicsSceneMouseEvent *e) noexcept
{
    m_rect_move_mode = true;
}

void MinimapRect::mouseMoveEvent(QGraphicsSceneMouseEvent *e) noexcept
{
    if (m_rect_move_mode)
        this->setPos(mapToScene(e->pos()));
}

void MinimapRect::mouseReleaseEvent(QGraphicsSceneMouseEvent *e) noexcept
{
    m_rect_move_mode = false;
}

