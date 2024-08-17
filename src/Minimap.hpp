#ifndef MINIMAP__HPP
#define MINIMAP__HPP

#include <QGraphicsView>
#include <QWidget>
#include <QShowEvent>
#include <QCloseEvent>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QVBoxLayout>
#include <QPixmap>
#include "MinimapRect.hpp"

class Minimap : public QGraphicsView
{
    Q_OBJECT
public:
    Minimap(QWidget *parent = nullptr);

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

    inline void setVisibleIfNotAutoHideMode(const bool &state) noexcept
    {
        if (!isVisible() && !m_auto_hide)
            this->setVisible(state);
        emit visibilityChanged(true);
    }

    inline void setSize(const QSize& size) noexcept
    {
        m_pix_scale_size = size;
    }

    inline void setSize(const QSize&& size) noexcept
    {
        m_pix_scale_size = size;
    }

    inline void setRectColor(const QString& color) noexcept
    {
        m_rectitem->setColor(color);
    }

    inline void setRectColor(const QString&& color) noexcept
    {
        m_rectitem->setColor(color);
    }

    inline void setRectFillColor(const QString& color) noexcept
    {
        m_rectitem->setFill(color);
    }

    inline void setRectFillColor(const QString&& color) noexcept
    {
        m_rectitem->setFill(color);
    }

    inline void setLocation(const Location& loc) noexcept
    {
        m_location = loc;
    }

    inline void setLocation(const Location&& loc) noexcept
    {
        m_location = loc;
    }

    inline void setRectAlpha(const float& alpha) noexcept
    {
        m_rectitem->setAlpha(alpha);
    }

    inline void setRectAlpha(const float&& alpha) noexcept
    {
        m_rectitem->setAlpha(alpha);
    }

    inline void setAutoHide(const bool& state) noexcept
    {
        m_auto_hide = state;
    }

    inline void setAutoHide(const bool&& state) noexcept
    {
        m_auto_hide = state;
    }

signals:
    void visibilityChanged(bool);

protected:
    inline void closeEvent(QCloseEvent *e) noexcept override
    {
        emit visibilityChanged(false);
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
