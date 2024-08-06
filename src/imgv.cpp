#include "imgv.hpp"
#include "src/AboutDialog.hpp"

IMGV::IMGV(argparse::ArgumentParser &parser, QWidget *parent)
    : QMainWindow(parent)
{
    // Set up the main widget and layout
    QWidget *centralWidget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout();
    QSplitter *splitter = new QSplitter();

    helpMenu->addAction(help__about);

    connect(help__about, &QAction::triggered, this, [&]() {
        AboutDialog *about = new AboutDialog(this);
        about->open();
    });

    centralWidget->setLayout(layout);

    splitter->addWidget(m_thumbnail_widget);
    splitter->addWidget(m_img_widget);

    splitter->setStretchFactor(1, 1);
    layout->addWidget(splitter);
    layout->addWidget(m_statusbar);
    setCentralWidget(centralWidget);

    centralWidget->setContentsMargins(0, 0, 0, 0);

    this->setContentsMargins(0, 0, 0, 0);

    m_slideshow_timer->setInterval(2 * 1000);
    connect(m_slideshow_timer, &QTimer::timeout, this, &IMGV::slideShow);

    connect(m_thumbnail_widget, &ThumbnailWidget::fileChangeRequested, m_img_widget, [&](QString filepath) {
        m_img_widget->loadFile(filepath);
    });

    parseCommandLineArguments(parser);
    initMenu();
    initConnections();
    initKeybinds();

    this->show();
}

void IMGV::initConfigDirectory()
{
    m_config_dir_path = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    auto config_dir = QDir(m_config_dir_path);
    m_sessions_dir_path = QString("%1%2sessions").arg(m_config_dir_path).arg(QDir::separator());
    auto sessions_dir = QDir(m_sessions_dir_path);

    if (!config_dir.exists())
        config_dir.mkdir(m_config_dir_path);

    QString script_file_path = QString("%1%2config.lua").arg(m_config_dir_path).arg(QDir::separator());
    QFile scriptFile(script_file_path);

    // If the script file doesn't exist, create an empty one
    if (!scriptFile.exists())
    {
        scriptFile.open(QIODevice::WriteOnly);
        scriptFile.write("-- This is the config file for IMGV\n");
        scriptFile.write("-- Check out the documentation for more information about configuring IMGV");
    }

    // create sessions folder
    
    if (!sessions_dir.exists())
        sessions_dir.mkdir(m_sessions_dir_path);

}

void IMGV::initMenu()
{

    setMenuBar(m_menuBar);

    fileMenu->addAction(file__openAction);
    fileMenu->addAction(file__openNewWindowAction);
    fileMenu->addMenu(file__openRecent);
    fileMenu->addMenu(file__openSession);
    fileMenu->addAction(file__saveSession);

    edit__rotate->addAction(rotate__clockwise);
    edit__rotate->addAction(rotate__anticlockwise);
    edit__rotate->addAction(rotate__custom);

    edit__flip->addAction(flip__horizontal);
    edit__flip->addAction(flip__vertical);

    editMenu->addMenu(edit__rotate);
    editMenu->addMenu(edit__flip);

    viewMenu->addAction(view__thumbnails);
    viewMenu->addAction(view__statusbar);
    viewMenu->addAction(view__menubar);
    viewMenu->addAction(view__maximize_image);

    view__maximize_image->setCheckable(true);

    view__thumbnails->setCheckable(true);
    view__thumbnails->setChecked(true);

    view__statusbar->setCheckable(true);
    view__statusbar->setChecked(true);

    view__menubar->setCheckable(true);
    view__menubar->setChecked(true);

    auto session_files = getSessionFiles();

    for(const auto &file: session_files)
    {
        QAction *action = new QAction(file);
        connect(action, &QAction::triggered, this, [&]() {
            auto filename = reinterpret_cast<QAction*>(sender())->text();
            QString file = QString("%1%2%3").arg(m_sessions_dir_path).arg(QDir::separator()).arg(filename);
            openSessionInNewWindow(file);
        });

        file__openSession->addAction(action);
    }

    connect(file__openAction, &QAction::triggered, this, &IMGV::openImage);
    connect(file__openNewWindowAction, &QAction::triggered, this, &IMGV::openImageInNewWindow);
    connect(m_img_widget, &ImageWidget::fileLoaded, m_statusbar, &StatusBar::updateFileInfo);

    connect(flip__horizontal, &QAction::triggered, m_img_widget, &ImageWidget::flipHorizontal);
    connect(flip__vertical, &QAction::triggered, m_img_widget, &ImageWidget::flipVertical);

    connect(rotate__clockwise, &QAction::triggered, m_img_widget, &ImageWidget::rotateClockwise);
    connect(rotate__anticlockwise, &QAction::triggered, m_img_widget, &ImageWidget::rotateAnticlockwise);
    connect(rotate__custom, &QAction::triggered, m_img_widget, [&]() {
        bool ok;
        qreal rotation = QInputDialog::getText(this, "Rotate image", "Enter angle to rotate image by", QLineEdit::Normal, "", &ok).toFloat();
        if (ok && rotation != 0.0)
            m_img_widget->rotate(rotation);

    });

    connect(view__thumbnails, &QAction::triggered, this, [&](bool state) {
        m_thumbnail_widget->setVisible(state);
    });

    connect(view__statusbar, &QAction::triggered, this, [&](bool state) {
        m_statusbar->setVisible(state);
    });

    connect(view__menubar, &QAction::triggered, this, [&](bool state) {
        m_menuBar->setVisible(state);
    });

    connect(file__saveSession, &QAction::triggered, this, &IMGV::saveSession);

    connect(view__maximize_image, &QAction::triggered, this, &IMGV::maximizeImage);

}

void IMGV::initKeybinds()
{
    QShortcut *kb_clockwise = new QShortcut(QKeySequence("."), this);
    QShortcut *kb_anticlockwise = new QShortcut(QKeySequence(","), this);
    QShortcut *kb_zoomin = new QShortcut(QKeySequence("="), this);
    QShortcut *kb_zoomout = new QShortcut(QKeySequence("-"), this);
    QShortcut *kb_zoomreset = new QShortcut(QKeySequence("0"), this);
    QShortcut *kb_fliphoriz = new QShortcut(QKeySequence(";"), this);
    QShortcut *kb_flipvert= new QShortcut(QKeySequence("'"), this);
    QShortcut *kb_fit_to_width = new QShortcut(QKeySequence("w"), this);
    QShortcut *kb_fit_to_height = new QShortcut(QKeySequence("h"), this);
    QShortcut *kb_toggle_menubar = new QShortcut(QKeySequence("Ctrl+M"), this);
    QShortcut *kb_goto_next = new QShortcut(QKeySequence("j"), this);
    QShortcut *kb_goto_prev = new QShortcut(QKeySequence("k"), this);
    QShortcut *kb_img_maximize = new QShortcut(QKeySequence("Shift+M"), this);

    connect(kb_clockwise, &QShortcut::activated, m_img_widget, &ImageWidget::rotateClockwise);
    connect(kb_anticlockwise, &QShortcut::activated, m_img_widget, &ImageWidget::rotateAnticlockwise);
    connect(kb_zoomin, &QShortcut::activated, m_img_widget, &ImageWidget::zoomIn);
    connect(kb_zoomout, &QShortcut::activated, m_img_widget, &ImageWidget::zoomOut);
    connect(kb_zoomreset, &QShortcut::activated, m_img_widget, &ImageWidget::zoomOriginal);
    connect(kb_fliphoriz, &QShortcut::activated, m_img_widget, &ImageWidget::flipHorizontal);
    connect(kb_flipvert, &QShortcut::activated, m_img_widget, &ImageWidget::flipVertical);
    connect(kb_fit_to_width, &QShortcut::activated, m_img_widget, &ImageWidget::fitToWidth);
    connect(kb_fit_to_height, &QShortcut::activated, m_img_widget, &ImageWidget::fitToHeight);
    connect(kb_toggle_menubar, &QShortcut::activated, this, [&]() {
        m_menuBar->setVisible(!m_menuBar->isVisible());
    });

    connect(kb_goto_next, &QShortcut::activated, m_thumbnail_widget, &ThumbnailWidget::gotoNext);
    connect(kb_goto_prev, &QShortcut::activated, m_thumbnail_widget, &ThumbnailWidget::gotoPrev);
    connect(kb_img_maximize, &QShortcut::activated, this, [&]() {
        view__maximize_image->setChecked(!m_image_maximize_mode);
        maximizeImage(!m_image_maximize_mode);
    });
}

void IMGV::initConnections()
{
    connect(m_thumbnail_widget, &ThumbnailWidget::itemDoubleClicked, this, [&](QListWidgetItem *item) {
        const QString text = item->data(Qt::UserRole).toString();
        m_img_widget->loadFile(text);
    });
}

void IMGV::openImageInNewWindow()
{
}

void IMGV::openImage()
{
    QStringList files = QFileDialog::getOpenFileNames(this, tr("Open Image"), "", tr("Images (*.png *.jpg *.bmp *.gif *.svg *.webp)"));
    if (!files.isEmpty()) {
        m_thumbnail_widget->createThumbnails(files);
        m_img_widget->loadFile(files[0]);
    }
}

void IMGV::slideShow()
{
}

QStringList IMGV::getSessionFiles()
{
    if (m_config_dir_path.isEmpty()) return {};

    return QDir(m_sessions_dir_path).entryList(QStringList() << "*.imgv" << "*.IMGV", QDir::Files);
}

void IMGV::saveSession()
{
    QStringList sessionFiles = getSessionFiles();
    bool ok;
    QString sessionName = QInputDialog::getText(this, "Save Session", "Please enter a name for the current session", QLineEdit::Normal, "", &ok) + ".imgv";
    if (ok && !sessionName.isEmpty())
    {
        if (sessionFiles.indexOf(sessionName) > -1)
        {
            QMessageBox::warning(this, "Error Saving Session", "A file with that name already exists. Please try again with different name");
            return;
        }

        // Save the session file
        
        QFile sess_file(QString("%1%2%3").arg(m_sessions_dir_path).arg(QDir::separator()).arg(sessionName));
        sess_file.open(QIODevice::WriteOnly);

        auto datetime = QString("%1\n").arg(QDateTime::currentDateTime().toString()).toStdString();
        sess_file.write(datetime.data());

        for(unsigned int i=0; i < m_thumbnail_widget->count(); i++)
        {
            QString filepath = m_thumbnail_widget->item(i)->data(Qt::UserRole).toString();
            sess_file.write(QString("%1\n").arg(filepath).toStdString().data());
        }
    }
    else {
        QMessageBox::critical(this, "Error", "Could not save the session file. Try again");
    }
}

void IMGV::readSessionFile(QString filename)
{
    QFile file(filename);
    if (!file.exists())
    {
        QMessageBox::critical(this, "File not found", "The specified session file was not found");
        return;
    }


    file.open(QIODevice::ReadOnly);
    QTextStream in(&file);

    QString date_modified = in.readLine();
    QStringList files;

    while (!in.atEnd())
        files.push_back(in.readLine());

    file.close();

    m_thumbnail_widget->createThumbnails(files);
    m_img_widget->loadFile(files[0]);
}

void IMGV::parseCommandLineArguments(argparse::ArgumentParser &parser)
{

    if (!parser.is_used("-n") && !parser.is_used("--no-config"))
    {
        initConfigDirectory();
    }

    if (parser.is_used("-l") || parser.is_used("--list-sessions"))
    {
        auto ses_files = getSessionFiles();
        qInfo() << "Session files found: " << ses_files.size();
        for(const QString &file: ses_files)
            qInfo() << file;
        exit(0);
    }

    if (parser.is_used("-s") || parser.is_used("--session"))
    {
        readSessionFile(QString::fromStdString(parser.get<std::vector<std::string>>("--session")[0]));
        return;
    }

    if (parser.is_used("--input") || parser.is_used("-i"))
    {
        auto files = parser.get<std::vector<std::string>>("--input");
        m_img_widget->loadFile(QString::fromStdString(files[0]));

        for(const auto &file: files)
            m_thumbnail_widget->addThumbnail(QString::fromStdString(file));

    }
}

void IMGV::openSessionInNewWindow(QString &file)
{
    QString program = QCoreApplication::applicationFilePath();

    QStringList arguments;
    arguments << "-s" << file;

    QProcess *process = new QProcess(this);
    process->startDetached();
    process->start(program, arguments);

    if (!process->waitForStarted())
    {
        qDebug() << "FAILED";
    }
    else {}
}

void IMGV::fullScreen()
{
    this->showFullScreen();
}

void IMGV::maximizeImage(bool state)
{
    m_image_maximize_mode = state;
    if (m_image_maximize_mode)
    {
        m_thumbnail_widget->setVisible(false);
        m_menuBar->setVisible(false);
        m_statusbar->setVisible(false);
    }
    else {
        m_thumbnail_widget->setVisible(true);
        m_menuBar->setVisible(true);
        m_statusbar->setVisible(true);
    }

}
