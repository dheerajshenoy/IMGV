#ifndef PIX_ANALYSER__HPP
#define PIX_ANALYSER__HPP

#include <QWidget>
#include <QGraphicsView>
#include <QLabel>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QGraphicsEllipseItem>
#include <QFormLayout>
#include <QColor>
#include <QShowEvent>
#include <QCloseEvent>
#include <QResizeEvent>
#include <QPushButton>

class PixAnalyser : public QWidget
{
    Q_OBJECT

public:
    explicit PixAnalyser(QWidget *parent = nullptr);
    ~PixAnalyser() {}

    void setPixmap(const QPixmap &img) noexcept;
    void analysePix(const QPointF &loc) noexcept;
    void setColorPicked(bool state) noexcept;

signals:
    void visibilityChanged(bool);
    void colorPicked();
    void pickColor();

protected:
    void showEvent(QShowEvent *e) noexcept override;
    void closeEvent(QCloseEvent *e) noexcept override;

private:
    QFormLayout *m_layout = new QFormLayout();
    QLabel *m_color_name = new QLabel();
    QLabel *m_color_rgb = new QLabel();
    QLabel *m_color_hsv = new QLabel();
    QLabel *m_color = new QLabel();
    QPushButton *m_pick_btn = new QPushButton("Pick Again");
    QPushButton *m_done_btn = new QPushButton("Done");
    QImage m_img;
};

#endif
