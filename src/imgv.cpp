#include "imgv.hpp"

IMGV::IMGV(argparse::ArgumentParser &parser, QWidget *parent)
    : QMainWindow(parent)
{


    file__openSession->setIcon(QIcon(":/icons/open-session.svg"));
    setWindowIcon(QIcon(":/icons/imgv.png"));
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

    splitter->setHandleWidth(0);

    splitter->addWidget(m_thumbnail_widget);
    splitter->addWidget(m_img_widget);

    splitter->setStretchFactor(1, 1);
    layout->addWidget(splitter);
    layout->addWidget(m_statusbar);
    setCentralWidget(centralWidget);

    layout->setContentsMargins(0, 0, 0, 0);
    centralWidget->setContentsMargins(0, 0, 0, 0);
    this->setContentsMargins(0, 0, 0, 0);

    /*m_slideshow_timer->setInterval(2 * 1000);*/
    /*connect(m_slideshow_timer, &QTimer::timeout, this, &IMGV::slideShow);*/

    connect(m_thumbnail_widget, &ThumbnailWidget::fileChangeRequested, m_img_widget, [&](QString filepath) {
        m_img_widget->loadFile(filepath);
    });

    connect(m_img_widget, &ImageWidget::fileDim, this, [&](int w, int h) {
        m_statusbar->setImgDimension(w, h);
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
    fileMenu->addAction(file__newSession);
    fileMenu->addAction(file__saveSession);
    fileMenu->addAction(file__closeSession);
    fileMenu->addAction(file__exit);

    edit__rotate->addAction(rotate__clockwise);
    edit__rotate->addAction(rotate__anticlockwise);
    edit__rotate->addAction(rotate__custom);
    edit__rotate->addAction(rotate__reset);

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

    toolsMenu->addAction(tools__manage_sessions);

    auto session_files = getSessionFiles();

    for(const auto &file: session_files)
    {
        QAction *action = new QAction(file);
        connect(action, &QAction::triggered, this, [&]() {
            auto filename = reinterpret_cast<QAction*>(sender())->text();
            QString file = QString("%1%2%3").arg(m_sessions_dir_path).arg(QDir::separator()).arg(filename);
            openSession(file);
            /*openSessionInNewWindow(file);*/
        });

        file__openSession->addAction(action);
    }

    connect(file__newSession, &QAction::triggered, this, &IMGV::newSession);
    connect(file__openAction, &QAction::triggered, this, &IMGV::openImage);
    connect(file__openNewWindowAction, &QAction::triggered, this, &IMGV::openImageInNewWindow);
    connect(m_img_widget, &ImageWidget::fileLoaded, m_statusbar, &StatusBar::updateFileInfo);
    connect(m_img_widget, &ImageWidget::droppedImage, m_thumbnail_widget, &ThumbnailWidget::addThumbnail);

    connect(file__closeSession, &QAction::triggered, this, &IMGV::closeSession);

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

    connect(rotate__reset, &QAction::triggered, m_img_widget, [&]() {
        m_img_widget->resetRotation();
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

    connect(file__exit, &QAction::triggered, this, &QApplication::exit);


    connect(m_thumbnail_widget, &ThumbnailWidget::visibilityChanged, this, [&](bool state) {
        view__thumbnails->setChecked(state);
    });

    connect(m_statusbar, &StatusBar::visibilityChanged, this, [&](bool state) {
        view__statusbar->setChecked(state);
    });

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
    QShortcut *kb_thumbnail_panel = new QShortcut(QKeySequence("T"), this);

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

    connect(kb_thumbnail_panel, &QShortcut::activated, this, [&]() {
        m_thumbnail_widget->setVisible(!m_thumbnail_widget->isVisible());
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
    QStringList files = QFileDialog::getOpenFileNames(this, tr("Open Image"), "", tr("Images (*.png *.jpg *.bmp *.gif *.svg *.webp)"));
    if (!files.isEmpty()) {

        QString program = QCoreApplication::applicationFilePath();
        QStringList arguments;
        arguments << "-i" << files;
        QProcess *process = new QProcess(this);
        process->startDetached();
        process->start(program, arguments);

        if (!process->waitForStarted())
            qDebug() << "FAILED";
    }
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
    m_statusbar->setMsg("Session Saved", 2);

    if (!m_session_name.isEmpty())
    {
        QFile sess_file(QString("%1%2%3.imgv").arg(m_sessions_dir_path).arg(QDir::separator()).arg(m_session_name));
        sess_file.open(QIODevice::WriteOnly);

        auto datetime = QString("%1\n").arg(QDateTime::currentDateTime().toString()).toStdString();
        sess_file.write(datetime.data());

        for(unsigned int i=0; i < m_thumbnail_widget->count(); i++)
        {
            QString filepath = m_thumbnail_widget->item(i)->data(Qt::UserRole).toString();
            sess_file.write(QString("%1\n").arg(filepath).toStdString().data());
        }

        return;
    }

    QStringList sessionFiles = getSessionFiles();
    bool ok;

    QString sessionName = QInputDialog::getText(this, "Save Session", "Please enter a name for the current session", QLineEdit::Normal, "", &ok) + ".imgv";
    if (ok && !sessionName.isEmpty())
    {
        if (sessionFiles.indexOf(sessionName) > -1)
        {
            QMessageBox::warning(this, "Overwrite", "A file with that name already exists. Please try again with different name");
            return;
        }

        QFile sess_file(QString("%1%2%3").arg(m_sessions_dir_path).arg(QDir::separator()).arg(sessionName));
        sess_file.open(QIODevice::WriteOnly);

        auto datetime = QString("%1\n").arg(QDateTime::currentDateTime().toString()).toStdString();
        sess_file.write(datetime.data());

        for(unsigned int i=0; i < m_thumbnail_widget->count(); i++)
        {
            QString filepath = m_thumbnail_widget->item(i)->data(Qt::UserRole).toString();
            sess_file.write(QString("%1\n").arg(filepath).toStdString().data());
        }

        m_session_name = sessionName;
        m_statusbar->setSessionName(sessionName);
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

    auto session_name = QFileInfo(file).baseName();
    m_session_name = session_name;
    m_statusbar->setSessionName(session_name);
}

void IMGV::parseCommandLineArguments(argparse::ArgumentParser &parser)
{

    if (!parser.is_used("--no-config"))
    {
        initConfigDirectory();
    }

    if (parser.is_used("--list-sessions"))
    {
        auto ses_files = getSessionFiles();
        qInfo() << "Session files found: " << ses_files.size();
        for(const QString &file: ses_files)
            qInfo() << file;
        exit(0);
    }

    if (parser.is_used("--new-session"))
    {
        auto ses_files = getSessionFiles();
        QString ses_name = QString::fromStdString(parser.get<std::string>("--new-session"));
        if (ses_files.indexOf(ses_name) > -1)
        {
            qInfo() << "Session file with that name already exists. Try again with different name";
            exit(0);
        }
        m_session_name = ses_name;
        m_statusbar->setMsg("Session file is not saved. Please do it manually", 5);
        m_statusbar->setSessionName(ses_name);
        /*saveSession();*/
        return;
    }

    if (parser.is_used("--session"))
    {
        readSessionFile(QString::fromStdString(parser.get<std::vector<std::string>>("--session")[0]));
        return;
    }

    if (parser.is_used("--input"))
    {
        auto files = parser.get<std::vector<std::string>>("--input");
        auto file = QString::fromStdString(files[0]);
        if (QFileInfo(file).isDir())
        {
            auto dirfiles = QDir(file).entryList(QStringList() << "*.jpg" << "*.svg" << "*.jpeg" << "*.webp" << "*.png" << "*.bmp" << "*.gif", QDir::Files);
            for(const auto &f: dirfiles)
                m_thumbnail_widget->addThumbnail(QString("%1%2%3").arg(file).arg(QDir::separator()).arg(f));
            return;
        }
        m_img_widget->loadFile(file);

        for(const auto &file: files)
            m_thumbnail_widget->addThumbnail(QString::fromStdString(file));
        return;
    }

    if (parser.is_used("files"))
    {
        auto files = parser.get<std::vector<std::string>>("files");
        auto file = QString::fromStdString(files[0]);
        if (QFileInfo(file).isDir())
        {
            auto dirfiles = QDir(file).entryList(QStringList() << "*.jpg" << "*.svg" << "*.jpeg" << "*.webp" << "*.png" << "*.bmp" << "*.gif", QDir::Files);
            for(const auto &f: dirfiles)
                m_thumbnail_widget->addThumbnail(QString("%1%2%3").arg(file).arg(QDir::separator()).arg(f));
            return;
        }
        m_img_widget->loadFile(file);

        for(const auto &file: files)
            m_thumbnail_widget->addThumbnail(QString::fromStdString(file));
        return;
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
        qDebug() << "FAILED";
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
        m_img_widget->setScrollBarsVisibility(false);
    }
    else {
        m_thumbnail_widget->setVisible(true);
        m_menuBar->setVisible(true);
        m_statusbar->setVisible(true);
        m_img_widget->setScrollBarsVisibility(true);
    }
}

void IMGV::closeSession()
{
    m_session_name.clear();
    m_thumbnail_widget->clear();
    m_img_widget->closeFile();
    m_statusbar->setSessionName("");
    m_statusbar->clearTexts();
    QPixmapCache::clear();
}

void IMGV::openSession(QString &file)
{
    if (m_session_name.isEmpty() && m_thumbnail_widget->count() == 0)
    {
        closeSession();
        readSessionFile(file);
    }
    else
    {
        QMessageBox msgBox;
        QAbstractButton *thisWindowBtn = msgBox.addButton("This Window", QMessageBox::YesRole);
        QAbstractButton *newWindowBtn = msgBox.addButton("New Window", QMessageBox::NoRole);
        msgBox.setText("There is already a session open in this window or there are images opened. Do you want to open the session in this window or in a new window ?");
        msgBox.setWindowTitle("Open Session");
        msgBox.exec();

        if (msgBox.clickedButton() == thisWindowBtn)
        {
            closeSession();
            readSessionFile(file);
        }
        else
            openSessionInNewWindow(file);
    }
}

void IMGV::newSession() noexcept
{
    QStringList sessions = getSessionFiles();
    QString sessionName;
    while (true)
    {
        sessionName = QInputDialog::getText(this, "New Session", "Please enter a name for the session");
        if (sessions.indexOf(sessionName + ".imgv") == -1)
            break;
    }
    QString program = QCoreApplication::applicationFilePath();

    QStringList arguments;
    arguments << "-S" << sessionName;

    QProcess *process = new QProcess(this);
    process->startDetached();
    process->start(program, arguments);

    if (!process->waitForStarted())
        qDebug() << "FAILED";
    else {}
}
