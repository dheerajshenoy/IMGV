#ifndef IMGV_HPP
#define IMGV_HPP

#include <qt6/QtWidgets/QApplication>
#include <qt6/QtWidgets/QMainWindow>
#include <qt6/QtWidgets/QSplitter>
#include <qt6/QtWidgets/QMenuBar>
#include <qt6/QtGui/QAction>
#include <qt6/QtWidgets/QFileDialog>
#include <qt6/QtWidgets/QGraphicsView>
#include <qt6/QtWidgets/QGraphicsScene>
#include <qt6/QtWidgets/QGraphicsPixmapItem>
#include <qt6/QtWidgets/QVBoxLayout>
#include <qt6/QtWidgets/QWidget>
#include "ThumbnailWidget.hpp"
#include "ImageWidget.hpp"
#include "StatusBar.hpp"

class IMGV : public QMainWindow
{
public:
    IMGV(QWidget *parent = nullptr);
    ~IMGV(){}

private:
    void initMenu();
    void initConnections();
    void openImage();

    ThumbnailWidget *m_thumbnail_widget = new ThumbnailWidget();
    ImageWidget *m_img_widget = new ImageWidget();
    StatusBar *m_statusbar = new StatusBar();
};


#endif
