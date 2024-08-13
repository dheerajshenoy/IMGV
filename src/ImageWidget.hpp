#ifndef IMAGE_WIDGET_HPP
#define IMAGE_WIDGET_HPP

#include <QGraphicsView>
#include <QScrollBar>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QPixmap>
#include <QPixmapCache>
#include <QFile>
#include <QImage>
#include <QWheelEvent>
#include <QDragMoveEvent>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QMimeDatabase>
#include <QMimeType>
#include <QFileInfo>
#include "MovieItem.hpp"
#include "utils.hpp"
#include "PixAnalyser.hpp"

class ImageWidget : public QGraphicsView
{
    Q_OBJECT

public:

    ImageWidget(QWidget *parent = nullptr);
    ~ImageWidget(){}

    void loadFile(QString file);
    void loadPixmap(QPixmap &pix);
    void closeFile();
    void zoomIn();
    void zoomOut();
    void zoomOriginal();
    void zoomFit();
    void flipVertical();
    void flipHorizontal();
    void rotate(qreal degrees);
    void rotateAnticlockwise();
    void rotateClockwise();
    void fitToWidth();
    void fitToWindow();
    void resetRotation() noexcept;
    void setScrollBarsVisibility(bool state) noexcept;
    void moveLeft() noexcept;
    void moveRight() noexcept;
    void moveUp() noexcept;
    void moveDown() noexcept;
    void resetScrollbars() noexcept;
    void setPixelAnalyser(PixAnalyser *e) noexcept;
    const QPixmap getPixmap() noexcept;

signals:
    void fileLoaded(QString);
    void droppedImage(QString);
    void fileDim(int, int);
    void mouseMoved(QPointF);

protected:
    void wheelEvent(QWheelEvent *e) override;
    void dragEnterEvent(QDragEnterEvent *e) override;
    void dragLeaveEvent(QDragLeaveEvent *e) override;
    void dragMoveEvent(QDragMoveEvent *e) override;
    void dropEvent(QDropEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) noexcept override;

private:
    QGraphicsScene *m_scene;
    QGraphicsPixmapItem *m_pixmapItem;
    MovieItem *m_movieItem;
    qreal m_zoomLevel = 1.0f, m_zoomFactor = 2.0f;
    qreal m_rotate = 0.0f;
    void updateView();

    void GifLoopHandler(int frameNumber);
    qreal scale() const;
    void setMatrix();

    Qt::AspectRatioMode m_aspect_ratio_mode = Qt::KeepAspectRatio;

    bool m_fit;

    bool m_horizontal_flip = false, m_vertical_flip = false;
    QMovie *m_movie;

    unsigned int m_gif_max_loop_count = 10;

    PixAnalyser *m_pix_analyser = nullptr;
};

#endif
