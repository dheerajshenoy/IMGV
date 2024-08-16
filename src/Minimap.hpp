#ifndef MINIMAP__HPP
#define MINIMAP__HPP

#include <QGraphicsView>
#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QVBoxLayout>
#include <QPixmap>
#include "MinimapRect.hpp"

class Minimap : public QGraphicsView
{
public:
    Minimap(QWidget *parent = nullptr);
    ~Minimap() {}

    enum Location
    {
        BottomRight,
        TopLeft,
        TopRight,
        BottomLeft,
        Custom
    };

    void setPixmap(const QPixmap &pix) noexcept;
    void updateRect(const QRectF rect) noexcept;
    Location location() noexcept { return m_location; }

    inline void setSize(const QSize size) noexcept
    {
        m_pix_scale_size = size;
    }

    inline void setRectColor(const QString color) noexcept
    {
        m_rectitem->setColor(color);
    }

    inline void setRectFillColor(const QString color) noexcept
    {
        m_rectitem->setFill(color);
    }

    inline void setLocation(const Location loc) noexcept
    {
        m_location = loc;
    }

    inline void setRectAlpha(const float alpha) noexcept
    {
        m_rectitem->setAlpha(alpha);
    }

    inline void setAutoHide(const bool state) noexcept
    {
        m_auto_hide = state;
    }

private:
    QVBoxLayout *m_layout = new QVBoxLayout();
    QGraphicsScene *m_scene = new QGraphicsScene();
    QGraphicsPixmapItem *m_pixitem = new QGraphicsPixmapItem;
    MinimapRect *m_rectitem = new MinimapRect();
    QPixmap m_pix;
    QRectF m_mainPixmapBoundingRect;
    QSize m_pix_scale_size;
    Location m_location = Location::BottomRight;
    bool m_auto_hide = false;
};

#endif
