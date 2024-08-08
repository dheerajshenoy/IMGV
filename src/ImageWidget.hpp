#ifndef IMAGE_WIDGET_HPP
#define IMAGE_WIDGET_HPP

#include <qt6/QtWidgets/QGraphicsView>
#include <qt6/QtWidgets/QGraphicsScene>
#include <qt6/QtWidgets/QGraphicsPixmapItem>
#include <qt6/QtGui/QPixmap>
#include <qt6/QtGui/QPixmapCache>
#include <qt6/QtCore/QFile>
#include <qt6/QtGui/QImage>
#include <qt6/QtGui/QWheelEvent>
#include <qt6/QtGui/QDragMoveEvent>
#include <qt6/QtGui/QDragEnterEvent>
#include <qt6/QtGui/QDragLeaveEvent>
#include <qt6/QtGui/QDropEvent>
#include <qt6/QtCore/QMimeData>
#include <qt6/QtCore/QMimeDatabase>
#include <qt6/QtCore/QMimeType>
#include <qt6/QtCore/QFileInfo>
#include "MovieItem.hpp"
#include "utils.hpp"

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
    void fitToHeight();
    void resetRotation() noexcept;
    void setScrollBarsVisibility(bool state) noexcept;
    void moveLeft() noexcept;
    void moveRight() noexcept;
    void moveUp() noexcept;
    void moveDown() noexcept;

signals:
    void fileLoaded(QString);
    void droppedImage(QString);
    void fileDim(int, int);

protected:
    void wheelEvent(QWheelEvent *e) override;
    void dragEnterEvent(QDragEnterEvent *e) override;
    void dragLeaveEvent(QDragLeaveEvent *e) override;
    void dragMoveEvent(QDragMoveEvent *e) override;
    void dropEvent(QDropEvent *e) override;

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

};

#endif
