#ifndef IMAGE_WIDGET_HPP
#define IMAGE_WIDGET_HPP

#include <QCursor>
#include <QLabel>
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
#include <QResizeEvent>
#include "MovieItem.hpp"
#include "utils.hpp"
#include "Minimap.hpp"
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
    const QPixmap getPixmap() noexcept;
    void setMinimapMode(const bool state) noexcept;
    void setMinimapRectColor(const QString color) noexcept;
    void setMinimapRectFillColor(const QString color) noexcept;
    void setMinimapRectAlpha(const float alpha) noexcept;
    void setMinimapAutoHide(const bool state) noexcept;
    void setMinimapSize(const QSize size) noexcept;
    void setPixAnalyseMode(const bool state, const bool closeDialog = true) noexcept;
    const QRectF visibleRect();
    void setVisibleRectFromMinimap(const QRectF rect) noexcept;
    void setZoomFactor(const qreal zoom) noexcept;
    void setHorizontalScrollFactor(const qreal factor) noexcept;
    void setVerticalScrollFactor(const qreal factor) noexcept;
    void setFitImageOnLoad(const bool fit) noexcept;
    void setMinimapLocation(const Minimap::Location loc) noexcept;

signals:
    void fileLoaded(QString);
    void droppedImage(QString);
    void fileDim(int, int);
    void mouseMoved(QPointF);
    void zoomChanged(qreal);
    void getRegion(QRectF);
    void pixAnalyserVisibilityChanged(bool);

protected:
    void wheelEvent(QWheelEvent *e) override;
    void dragEnterEvent(QDragEnterEvent *e) override;
    void dragLeaveEvent(QDragLeaveEvent *e) override;
    void dragMoveEvent(QDragMoveEvent *e) override;
    void dropEvent(QDropEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) noexcept override;
    void mousePressEvent(QMouseEvent *e) noexcept override;
    void mouseReleaseEvent(QMouseEvent *e) noexcept override;
    void resizeEvent(QResizeEvent *e) noexcept override;

private:
    QGraphicsScene *m_scene = nullptr;
    QGraphicsPixmapItem *m_pixmapItem = nullptr;
    MovieItem *m_movieItem = nullptr;
    qreal m_zoomLevel = 1.0f, m_zoomFactor = 2.0f;
    qreal m_rotate = 0.0f;
    qreal m_horizontal_scroll_factor = 20.0, m_vertical_scroll_factor = 20.0;
    void updateView();
    void GifLoopHandler(int frameNumber);
    qreal scale() const;
    void setMatrix();
    Qt::AspectRatioMode m_aspect_ratio_mode = Qt::KeepAspectRatio;
    bool m_fit;
    bool m_horizontal_flip = false, m_vertical_flip = false;
    QMovie *m_movie = nullptr;
    unsigned int m_gif_max_loop_count = 10;
    PixAnalyser *m_pix_analyser = nullptr;
    bool m_pix_analyse_mode = false, m_minimap_mode = false;
    bool m_panning = false;
    bool m_fit_image_on_load = true;
    Minimap *m_minimap = new Minimap(this);
};

#endif
