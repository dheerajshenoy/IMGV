#ifndef MINIMAP__HPP
#define MINIMAP__HPP

#include <QGraphicsView>
#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QVBoxLayout>
#include <QPixmap>
#include "MinimapRect.hpp"

class Minimap : public QWidget
{
    Q_OBJECT
public:
    Minimap(QWidget *parent = nullptr);
    ~Minimap() {}
    void setPixmap(const QPixmap &pix) noexcept;
    void updateRect(const QRectF rect) noexcept;
    void setMainPixmapBoundingRect(const QRectF &rect) noexcept;
    void setRectColor(const QString color) noexcept;
    void setRectSize(const QSize size) noexcept;

signals:
    void changeMainRegion(QRectF);
    void rectMoved(QRectF);

private:
    QVBoxLayout *m_layout = new QVBoxLayout();
    QGraphicsView *m_gv = new QGraphicsView();
    QGraphicsScene *m_scene = new QGraphicsScene();
    QGraphicsPixmapItem *m_pixitem = new QGraphicsPixmapItem;
    MinimapRect *m_rectitem = new MinimapRect();
    QPixmap m_pix;
    QRectF m_mainPixmapBoundingRect;
    QSize m_pix_scale_size;
};

#endif
