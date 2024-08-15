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
    void setFill(const QString color) noexcept;
    void setAlpha(const float alpha) noexcept;

private:
    QColor m_fill_color;
};

#endif
