#ifndef IMAGE_WIDGET_HPP
#define IMAGE_WIDGET_HPP

#include <qt6/QtWidgets/QGraphicsView>
#include <qt6/QtWidgets/QGraphicsScene>
#include <qt6/QtWidgets/QGraphicsPixmapItem>
#include <qt6/QtGui/QPixmap>
#include <qt6/QtCore/QFile>
#include <qt6/QtGui/QImage>
#include <qt6/QtGui/QWheelEvent>

class ImageWidget : public QGraphicsView
{
    Q_OBJECT

public:
    ImageWidget(QWidget *parent = nullptr);
    ~ImageWidget(){}

    void loadFile(QString file);
    void setPixmap(const QString &filePath);
    void zoomIn(int);
    void zoomOut(int);
    void zoomOriginal();
    void zoomFit();

protected:
    void wheelEvent(QWheelEvent *e) override;

private:
    QGraphicsScene *m_scene;
    QGraphicsPixmapItem *m_pixmapItem;
    qreal m_zoomLevel = 1.0f;
    void updateView();

    qreal scale() const;
    void setMatrix();

    Qt::AspectRatioMode m_aspect_ratio_mode = Qt::KeepAspectRatio;

    bool m_fit;
};

#endif
