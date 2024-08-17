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

    inline void setColor(const QString &color) noexcept
    {
        if (QColor(color).isValid())
            this->setPen(QPen(QColor::fromString(color)));
    }

    inline void setColor(QString&& color) noexcept
    {
        if (QColor(color).isValid())
            this->setPen(QPen(QColor::fromString(color)));
    }

    inline void setFill(const QString& color) noexcept
    {
        if (QColor(color).isValid())
        {
            m_fill_color = QColor::fromString(color);
            this->setBrush(QBrush(m_fill_color));
        }
    }

    inline void setFill(QString&& color) noexcept
    {
        if (QColor(color).isValid())
        {
            m_fill_color = QColor::fromString(color);
            this->setBrush(QBrush(m_fill_color));
        }
    }

    inline void setAlpha(const float &alpha) noexcept
    {
        this->setOpacity(alpha);
    }

    inline void setAlpha(float&& alpha) noexcept
    {
        this->setOpacity(alpha);
    }

private:
    QColor m_fill_color;
};

#endif
