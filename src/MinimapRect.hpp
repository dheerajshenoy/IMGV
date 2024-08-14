#ifndef MINIMAP_RECT__HPP
#define MINIMAP_RECT__HPP

#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>
#include <QMouseEvent>

class MinimapRect : public QGraphicsRectItem
{
public:
    MinimapRect(QGraphicsItem *parent = nullptr);
    ~MinimapRect() {}

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *e) noexcept override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *e) noexcept override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *e) noexcept override;

private:
    bool m_rect_move_mode = false;
};

#endif
