#ifndef PIX_ANALYSER__HPP
#define PIX_ANALYSER__HPP

#include <QWidget>
#include <QGraphicsView>
#include <QLabel>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QGraphicsEllipseItem>
#include <QGridLayout>
#include <QColor>
#include <QHideEvent>
#include <QShowEvent>
#include <QResizeEvent>

class PixAnalyser : public QWidget
{
    Q_OBJECT

public:
    explicit PixAnalyser(QWidget *parent = nullptr);
    ~PixAnalyser() {}

    void setPixmap(const QPixmap &img) noexcept;
    void analysePix(const QPointF &loc) noexcept;

signals:
    void visibilityChanged(bool);

protected:
    void showEvent(QShowEvent *e) noexcept override;
    void hideEvent(QHideEvent *e) noexcept override;

private:
    QGridLayout *m_layout = new QGridLayout();
    QLabel *m_color_name = new QLabel();
    QLabel *m_color_rgb = new QLabel();
    QLabel *m_color_hsv = new QLabel();
    QLabel *m_color = new QLabel();
    QImage m_img;
};

#endif
