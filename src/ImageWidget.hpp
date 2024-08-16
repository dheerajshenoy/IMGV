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
#include "Thumbnail.hpp"

class ImageWidget : public QGraphicsView
{
    Q_OBJECT

public:

    ImageWidget(QWidget *parent = nullptr);
    ~ImageWidget(){}

    void loadFile(QString file) noexcept;
    void closeFile() noexcept;
    void zoomIn() noexcept;
    void zoomOut() noexcept;
    void zoomOriginal() noexcept;
    void zoomFit() noexcept;
    void flipVertical() noexcept;
    void flipHorizontal() noexcept;
    void rotate(qreal degrees) noexcept;
    void fitToWidth() noexcept;
    void fitToWindow() noexcept;
    void resetRotation() noexcept;
    void setScrollBarsVisibility(bool state) noexcept;
    void moveLeft() noexcept;
    void moveRight() noexcept;
    void moveUp() noexcept;
    void moveDown() noexcept;
    void resetScrollbars() noexcept;
    void setMinimapMode(const bool state) noexcept;
    void setPixAnalyseMode(const bool state, const bool closeDialog = true) noexcept;
    const QRectF visibleRect() noexcept;
    void setVisibleRectFromMinimap(const QRectF rect) noexcept;


    inline void setHorizontalScrollFactor(const qreal factor) noexcept
    {
        m_horizontal_scroll_factor = factor;
    }

    inline void setVerticalScrollFactor(const qreal factor) noexcept
    {
        m_vertical_scroll_factor = factor;
    }

    inline void setZoomFactor(const qreal zoom) noexcept
    {
        m_zoomFactor = zoom;
    }

    inline void setFitImageOnLoad(const bool fit) noexcept
    {
        m_fit_image_on_load = fit;
    }


    inline void setMinimapRectColor(const QString color) noexcept
    {
        m_minimap->setRectColor(color);
    }

    inline void setMinimapRectFillColor(const QString color) noexcept
    {
        if (color.isEmpty()) return;
        m_minimap->setRectFillColor(color);
    }

    inline void setMinimapRectAlpha(const float alpha) noexcept
    {
        m_minimap->setRectAlpha(alpha);
    }

    inline void setMinimapAutoHide(const bool state) noexcept
    {
        m_minimap->setAutoHide(state);
    }

    inline void setMinimapSize(const QSize size) noexcept
    {
        m_minimap->setSize(size);
    }

    inline void setMinimapLocation(const Minimap::Location loc) noexcept
    {
        m_minimap->setLocation(loc);
    }

    inline const QPixmap getPixmap() noexcept
    {
        return m_pixmapItem->pixmap();
    }

    inline void rotateAnticlockwise() noexcept
    {
        rotate(-90);
    }

    inline void rotateClockwise() noexcept
    {
        rotate(90);
    }


signals:
    void fileLoaded(QString);
    void droppedImage(QString);
    void fileDim(int, int);
    void mouseMoved(QPointF);
    void zoomChanged(qreal);
    void getRegion(QRectF);
    void pixAnalyserVisibilityChanged(bool);
    void minimapVisibilityChanged(bool);

protected:
    void wheelEvent(QWheelEvent *e) noexcept override;
    void dragEnterEvent(QDragEnterEvent *e) noexcept override;
    void dragLeaveEvent(QDragLeaveEvent *e) noexcept override;
    void dragMoveEvent(QDragMoveEvent *e) noexcept override;
    void dropEvent(QDropEvent *e) noexcept override;
    void mouseMoveEvent(QMouseEvent *e) noexcept override;
    void mousePressEvent(QMouseEvent *e) noexcept override;
    void mouseReleaseEvent(QMouseEvent *e) noexcept override;
    void resizeEvent(QResizeEvent *e) noexcept override;

private:

    void updateView() noexcept;
    void GifLoopHandler(int frameNumber) noexcept;
    qreal scale() const noexcept;
    void setMatrix() noexcept;

    QGraphicsScene *m_scene = nullptr;
    QGraphicsPixmapItem *m_pixmapItem = nullptr;
    MovieItem *m_movieItem = nullptr;
    qreal m_zoomLevel = 1.0f, m_zoomFactor = 2.0f;
    qreal m_rotate = 0.0f;
    qreal m_horizontal_scroll_factor = 20.0, m_vertical_scroll_factor = 20.0;
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
