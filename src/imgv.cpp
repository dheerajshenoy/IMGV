#include "imgv.hpp"

IMGV::IMGV(QWidget *parent)
    : QMainWindow(parent)
{
    // Set up the main widget and layout
    QWidget *centralWidget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout();
    QSplitter *splitter = new QSplitter();

    centralWidget->setLayout(layout);

    splitter->addWidget(m_thumbnail_widget);
    splitter->addWidget(m_img_widget);

    splitter->setStretchFactor(1, 1);
    layout->addWidget(splitter);
    layout->addWidget(m_statusbar);
    setCentralWidget(centralWidget);

    initMenu();
    initConnections();

    this->show();
}

void IMGV::initMenu()
{
    QMenuBar *menuBar = new QMenuBar();
    setMenuBar(menuBar);

    QMenu *fileMenu = menuBar->addMenu("&File");

    QAction *openAction = new QAction("&Open");
    fileMenu->addAction(openAction);
    
    connect(openAction, &QAction::triggered, this, &IMGV::openImage);
}

void IMGV::initConnections()
{
    connect(m_thumbnail_widget, &ThumbnailWidget::itemDoubleClicked, this, [&](QListWidgetItem *item) {
        const QString text = item->data(Qt::UserRole).toString();
        m_img_widget->loadFile(text);
        m_statusbar->updateFileInfo(text);
    });
}

void IMGV::openImage()
{
    QStringList files = QFileDialog::getOpenFileNames(this, tr("Open Image"), "", tr("Images (*.png *.jpg *.bmp)"));
    if (!files.isEmpty()) {
        m_thumbnail_widget->createThumbnails(files);
        m_img_widget->loadFile(files[0]);
    }
}


