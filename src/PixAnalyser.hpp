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
#include <QLineEdit>
#include <QPushButton>
#include <QApplication>
#include <QMouseEvent>
#include <QEnterEvent>
#include <QToolTip>
#include <Magick++.h>

class PixLineEdit : public QLineEdit
{
public:
    PixLineEdit(QWidget *parent = nullptr)
        : QLineEdit(parent)
    {
        this->setReadOnly(true);
    }
    
protected:
    void mouseDoubleClickEvent(QMouseEvent *e) override
    {
        this->selectAll();
        this->copy();
        this->deselect();
    }
};

class PixAnalyser : public QWidget
{
    Q_OBJECT

public:
    explicit PixAnalyser(QWidget *parent = nullptr);

    void analysePix(const QPointF &loc) const noexcept;

    inline void setColorPicked(const bool &state) const noexcept
    {
        m_pick_btn->setVisible(true);
    }
    inline void setPixmap(const QPixmap &pix) noexcept
    {
        m_img = pix.toImage();
    }


signals:
    void visibilityChanged(bool);
    void colorPicked();
    void pickColor();

protected:

    inline void showEvent(QShowEvent *e) noexcept override
    {
        m_pick_btn->setVisible(false);
        emit visibilityChanged(true);
    }

    inline void closeEvent(QCloseEvent *e) noexcept override
    {
        emit visibilityChanged(false);
    }

private:
    QFormLayout *m_layout = new QFormLayout();
    PixLineEdit *m_pos = new PixLineEdit();
    PixLineEdit *m_color_hex = new PixLineEdit();
    PixLineEdit *m_color_rgb = new PixLineEdit();
    PixLineEdit *m_color_hsv = new PixLineEdit();
    PixLineEdit *m_color_cmyk = new PixLineEdit();
    PixLineEdit *m_color_hsl = new PixLineEdit();
    QLabel *m_color = new QLabel();
    QLabel *m_copy_info = new QLabel("Double click to copy color to clipboard");
    QPushButton *m_pick_btn = new QPushButton("Pick Again");
    QPushButton *m_done_btn = new QPushButton("Done");
    QImage m_img;
};

#endif
