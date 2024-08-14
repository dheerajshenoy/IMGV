#ifndef MINIMAP_RECT__HPP
#define MINIMAP_RECT__HPP

#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>
#include <QMouseEvent>
#include <QObject>
#include <QPen>

class MinimapRect : public QGraphicsRectItem
{
public:
    MinimapRect(QGraphicsItem *parent = nullptr);
    ~MinimapRect() {}

    void setColor(const QString color) noexcept;
};

#endif
