#include "imgv.hpp"

IMGV::IMGV(argparse::ArgumentParser &parser, QWidget *parent)
    : QMainWindow(parent)
{

    // 50 MB cache limit
    /*QPixmapCache::setCacheLimit(10240 * 5);*/
    /*file__openSession->setIcon(QIcon(":/icons/open-session.svg"));*/
    setWindowIcon(QIcon(":/resources/images/logo.svg"));
    // Set up the main widget and layout
    QWidget *centralWidget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout();
    QSplitter *splitter = new QSplitter();

    m_note_holder->setAcceptRichText(true);
    /*m_thumbnail_view->setModel(m_thumbnail_model);*/
    helpMenu->addAction(help__about);

    connect(help__about, &QAction::triggered, this, [&]() {
        AboutDialog *about = new AboutDialog(this);
        about->open();
    });

    centralWidget->setLayout(layout);

    m_note_holder->setVisible(false);

    splitter->setHandleWidth(0);
    splitter->addWidget(m_left_pane);

    m_left_pane_layout = new QVBoxLayout();
    m_left_pane->setLayout(m_left_pane_layout);

    m_left_pane_layout->addWidget(m_thumbnail_view);
    m_right_pane->setLayout(m_right_pane_layout);

    m_left_pane_layout->setContentsMargins(0, 0, 0, 0);
    m_right_pane_layout->setContentsMargins(0, 0, 0, 0);

    m_right_pane_splitter->addWidget(m_img_widget);
    m_right_pane_splitter->addWidget(m_note_holder);

    connect(m_note_holder, &NoteWidget::modificationChanged, m_statusbar, &StatusBar::setNoteModified);

    /*connect(m_note_holder, &NoteWidget::visibilityChanged, m_statusbar, &StatusBar::modificationLabelVisiblity);*/

    m_right_pane_splitter->setStretchFactor(0, 1);
    m_right_pane_layout->addWidget(m_right_pane_splitter);

    m_thumbnail_search_edit->setVisible(false);
    splitter->addWidget(m_right_pane);

    connect(m_img_widget, &ImageWidget::zoomChanged, this, [&](qreal zoom) {
        m_statusbar->setZoom("Zoom: " + QString::number(4 * zoom));
    });

    connect(m_thumbnail_search_edit, &QLineEdit::textChanged, this, &IMGV::ThumbSearchTextChanged);
    connect(m_thumbnail_search_edit, &QLineEdit::returnPressed, this, [&]() {
        m_thumbnail_view->searchMode(false);
        m_thumbnail_search_edit->setVisible(false);
    });

    splitter->setStretchFactor(1, 1);
    layout->addWidget(splitter);
    layout->addWidget(m_thumbnail_search_edit);
    layout->addWidget(m_statusbar);
    setCentralWidget(centralWidget);

    layout->setContentsMargins(5, 5, 5, 5);
    centralWidget->setContentsMargins(0, 0, 0, 0);
    this->setContentsMargins(0, 0, 0, 0);


    connect(m_thumbnail_view, &ThumbnailView::fileChangeRequested, m_img_widget, [&](QString filepath) {
        m_img_widget->loadFile(filepath);
        m_note_holder->setMarkdown(m_thumbnail_view->currentIndex().data(Thumbnail::Note).toString());
    });

    connect(m_img_widget, &ImageWidget::fileDim, this, [&](int w, int h) {
        m_statusbar->setImgDimension(w, h);
    });

    parseCommandLineArguments(parser);
    initMenu();
    initConnections();
    this->show();
}

void IMGV::initDefaultConfig()
{
    m_statusbar->setSpacing(10);
    for(const auto &s : QStringList() << "message" << "path" << "note-indicator" << "note-modified-indicator" << "stretch" << "size" << "dimension" << "session")
        m_statusbar->addWidget(s);
    initKeybinds();
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

    m_lua_state.open_libraries(sol::lib::io, sol::lib::base, sol::lib::table);

    try
    {
        m_lua_state.safe_script_file(script_file_path.toStdString());
    }
    catch (const sol::error &e)
    {
        QMessageBox::critical(this, "Error Loading Config File", "There was a problem loading config.lua file");
        return;
    }

    sol::optional<sol::object> defaults_table_exist = m_lua_state["Defaults"];
    if (defaults_table_exist)
    {
        sol::table defaults_table = defaults_table_exist.value();
        m_menuBar->setVisible(defaults_table["menubar"].get_or(true));
        m_img_widget->setZoomFactor(defaults_table["zoom_factor"].get_or(2.0));
        auto fit_on_load = defaults_table["fit_on_load"].get_or(false);

        m_img_widget->setFitImageOnLoad(fit_on_load);
        edit__fit_on_load->setChecked(fit_on_load);

        sol::optional<sol::table> scroll_factor_table_optional = defaults_table["scroll_factor"];

        if (scroll_factor_table_optional)
        {
            sol::table scroll_factor_table = scroll_factor_table_optional.value();
            m_img_widget->setHorizontalScrollFactor(scroll_factor_table["horizontal"].get_or(20.0));
            m_img_widget->setVerticalScrollFactor(scroll_factor_table["vertical"].get_or(20.0));
        }

        // Thumbnails Table
        sol::optional<sol::table> thumbnails_table_exist = defaults_table["thumbnails"];
        if (thumbnails_table_exist)
        {
            sol::table thumbnails_table = thumbnails_table_exist.value();

            m_thumbnail_view->setVisible(thumbnails_table["show"].get_or(true));
            m_thumbnail_view->setUniformItemSizes(thumbnails_table["uniform"].get_or(false));
            
            auto mode = thumbnails_table["mode"].get_or<std::string>("icon");

            if (mode == "icon")
                m_thumbnail_view->setViewMode(QListView::IconMode);
            else if (mode == "list")
                m_thumbnail_view->setViewMode(QListView::ListMode);
            else
                m_thumbnail_view->setViewMode(QListView::IconMode);

            auto selection_mode = thumbnails_table["selection"].get_or<std::string>("extended");

            if (selection_mode == "extended")
                m_thumbnail_view->setSelectionMode(QListView::SelectionMode::ExtendedSelection);
            else if (selection_mode == "contiguous")
                m_thumbnail_view->setSelectionMode(QListView::SelectionMode::ContiguousSelection);
            else if (selection_mode == "single")
                m_thumbnail_view->setSelectionMode(QListView::SelectionMode::SingleSelection);
            else if (selection_mode == "multi")
                m_thumbnail_view->setSelectionMode(QListView::SelectionMode::MultiSelection);
            else
                m_thumbnail_view->setSelectionMode(QListView::SelectionMode::ExtendedSelection);

            
            m_thumbnail_view->setSpacing(thumbnails_table["spacing"].get_or(20));  // Spacing between thumbnails
            
            // Icon Size
            sol::optional<sol::table> icon_size_table_optional = thumbnails_table["icon_size"];
            if (icon_size_table_optional)
            {
                auto icon_size_table = icon_size_table_optional.value();

                m_thumbnail_view->setIconSize(QSize(icon_size_table["width"].get_or(100), icon_size_table["height"].get_or(100)));  // Set the size for thumbnails
            }

            auto elide = thumbnails_table["text_elide"].get_or<std::string>("none");

            if (elide == "none")
                m_thumbnail_view->setTextElideMode(Qt::TextElideMode::ElideNone);
            else if (elide == "right")
                m_thumbnail_view->setTextElideMode(Qt::TextElideMode::ElideRight);
            else if (elide == "left")
                m_thumbnail_view->setTextElideMode(Qt::TextElideMode::ElideLeft);
            else if (elide == "middle")
                m_thumbnail_view->setTextElideMode(Qt::TextElideMode::ElideMiddle);
            else
                m_thumbnail_view->setTextElideMode(Qt::TextElideMode::ElideNone);

            auto resize = thumbnails_table["resize"].get_or(false);
            if (resize)
            {
                m_thumbnail_view->setResizeMode(QListView::Adjust);
            }
            else
            {
                m_thumbnail_view->setResizeMode(QListView::Fixed);
                m_thumbnail_view->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);
            }

        }

        m_auto_notes_popup = defaults_table["auto_notes_popup"].get_or(false);

        m_img_widget->setScrollBarsVisibility(defaults_table["scrollbars"].get_or(true));

        if(defaults_table["start_in_minimal_mode"].get_or(false))
            maximizeImage();

        // keybindings table
        sol::optional<sol::table> keybindings_table_exist = defaults_table["keybindings"];
        if (keybindings_table_exist)
        {
            sol::table keys_table = keybindings_table_exist.value();

            QMap<QString, QString> keys;
            for(auto &pair : keys_table)
            {
                QString action = pair.first.as<std::string>().c_str();
                QString key = pair.second.as<std::string>().c_str();
                keys[action] = key;
            }

            for(auto it = keys.begin(); it != keys.end(); it++)
            {
                QString action = it.key();
                QString key = it.value();

                QShortcut *shortcut = new QShortcut(QKeySequence(key), this);

                if (action == "zoom_in") {
                    QObject::connect(shortcut, &QShortcut::activated, m_img_widget, &ImageWidget::zoomIn);
                } else if (action == "zoom_out") {
                    QObject::connect(shortcut, &QShortcut::activated, m_img_widget, &ImageWidget::zoomOut);
                } else if (action == "zoom_reset") {
                    QObject::connect(shortcut, &QShortcut::activated, m_img_widget, &ImageWidget::zoomOriginal);
                } else if (action == "rotate_clockwise") {
                    QObject::connect(shortcut, &QShortcut::activated, m_img_widget, &ImageWidget::rotateClockwise);
                } else if (action == "rotate_anticlockwise") {
                    QObject::connect(shortcut, &QShortcut::activated, m_img_widget, &ImageWidget::rotateAnticlockwise);
                } else if (action == "flip_vertical") {
                    QObject::connect(shortcut, &QShortcut::activated, m_img_widget, &ImageWidget::flipVertical);
                } else if (action == "flip_horizontal") {
                    QObject::connect(shortcut, &QShortcut::activated, m_img_widget, &ImageWidget::flipHorizontal);
                } else if (action == "left") {
                    QObject::connect(shortcut, &QShortcut::activated, m_img_widget, &ImageWidget::moveLeft);
                } else if (action == "down") {
                    QObject::connect(shortcut, &QShortcut::activated, m_img_widget, &ImageWidget::moveDown);
                } else if (action == "up") {
                    QObject::connect(shortcut, &QShortcut::activated, m_img_widget, &ImageWidget::moveUp);
                } else if (action == "right") {
                    QObject::connect(shortcut, &QShortcut::activated, m_img_widget, &ImageWidget::moveRight);
                } else if (action == "next") {
                    QObject::connect(shortcut, &QShortcut::activated, m_thumbnail_view, &ThumbnailView::gotoNext);
                } else if (action == "prev") {
                    QObject::connect(shortcut, &QShortcut::activated, m_thumbnail_view, &ThumbnailView::gotoPrev);
                } else if (action == "notes") {
                    QObject::connect(shortcut, &QShortcut::activated, this, &IMGV::toggleNotes);
                } else if (action == "maximize") {
                    QObject::connect(shortcut, &QShortcut::activated, this, &IMGV::maximizeImage);
                } else if (action == "search") {
                    QObject::connect(shortcut, &QShortcut::activated, this, &IMGV::searchThumbnails);
                } else if (action == "toggle_menubar") {
                    QObject::connect(shortcut, &QShortcut::activated, this, &IMGV::toggleMenubar);
                } else if (action == "toggle_statusbar") {
                    QObject::connect(shortcut, &QShortcut::activated, this, &IMGV::toggleStatusbar);
                } else if (action == "toggle_thumbnail_panel") {
                    QObject::connect(shortcut, &QShortcut::activated, this, &IMGV::toggleThumbnailPanel);
                } else if (action == "fit_width") {
                    QObject::connect(shortcut, &QShortcut::activated, m_img_widget, &ImageWidget::fitToWidth);
                } else if (action == "fit_window") {
                    QObject::connect(shortcut, &QShortcut::activated, m_img_widget, &ImageWidget::fitToWindow);
                }
            }
        }
        else 
        {
            // Load default keybindings if custom keybindings not found
            if (defaults_table["default_keybindings"].get_or(true))
                initKeybinds();
        }


        // minimap table
        sol::optional<sol::table> minimap_table_exists = defaults_table["minimap"];
        if (minimap_table_exists)
        {
            sol::table minimap_table = minimap_table_exists.value();
            m_minimap_rect_color = QString::fromStdString(minimap_table["rect_color"].get_or<std::string>("#000000"));


            sol::optional<sol::table> minimap_size_table_optional = minimap_table["size"];
            if (minimap_size_table_optional)
            {
                auto minimap_size_table = minimap_size_table_optional.value();

                m_minimap_rect_size = QSize(minimap_size_table["width"].get_or(100), minimap_size_table["height"].get_or(100));  // Set the size for thumbnails
            }
        }


        // statusbar table
        sol::optional<sol::table> statusbar_table_exists = defaults_table["statusbar"];

        if (statusbar_table_exists)
        {
            sol::table statusbar_table = statusbar_table_exists.value();
            m_statusbar->setVisible(statusbar_table["show"].get_or(true));
            auto spacing = statusbar_table["spacing"].get_or(10);

            auto note_indicator = statusbar_table["note_indicator"].get_or<std::string>("NOTE");
            auto note_modified_indicator = statusbar_table["note_modified_indicator"].get_or<std::string>("[M]");

            m_statusbar->setNoteIndicator(QString::fromStdString(note_indicator));
            m_statusbar->setNoteModifiedIndicator(QString::fromStdString(note_modified_indicator));

            m_statusbar->setSpacing(spacing);

            sol::optional<sol::table> elements_table_exists = statusbar_table["elements"];

            if (elements_table_exists)
            {
                sol::table elements = elements_table_exists.value();

                for(const auto &element : elements)
                m_statusbar->addWidget(QString::fromStdString(element.second.as<std::string>()));
            }

        }

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

    edit__zoom->addAction(zoom__in);
    edit__zoom->addAction(zoom__out);
    edit__zoom->addAction(zoom__reset);

    editMenu->addMenu(edit__rotate);
    editMenu->addMenu(edit__flip);
    editMenu->addMenu(edit__zoom);
    editMenu->addAction(edit__fit_on_load);

    edit__fit_on_load->setCheckable(true);

    viewMenu->addAction(view__minimap);
    viewMenu->addAction(view__thumbnails);
    viewMenu->addAction(view__statusbar);
    viewMenu->addAction(view__menubar);
    viewMenu->addAction(view__notes);
    viewMenu->addAction(view__maximize_image);

    view__maximize_image->setCheckable(true);
    view__thumbnails->setCheckable(true);
    view__statusbar->setCheckable(true);
    view__menubar->setCheckable(true);
    view__notes->setCheckable(true);
    view__minimap->setCheckable(true);

    toolsMenu->addAction(tools__manage_sessions);
    toolsMenu->addAction(tools__pix_analyser);
    toolsMenu->addAction(tools__slideshow);

    tools__pix_analyser->setCheckable(true);
    tools__slideshow->setCheckable(true);

    connect(edit__fit_on_load, &QAction::triggered, [&](bool state) {
        m_img_widget->setFitImageOnLoad(state);
    });

    connect(zoom__in, &QAction::triggered, m_img_widget, &ImageWidget::zoomIn);
    connect(zoom__out, &QAction::triggered, m_img_widget, &ImageWidget::zoomOut);
    connect(zoom__reset, &QAction::triggered, m_img_widget, &ImageWidget::zoomOriginal);

    connect(tools__slideshow, &QAction::triggered, this, [&](bool state) {
        if (state)
        {
            if (!m_slideshow_timer)
                m_slideshow_timer = new QTimer();
            m_slideshow_timer->setInterval(2 * 1000);
            m_slideshow_index = -1;
            m_slideshow_files = m_thumbnail_view->getAllFiles();
            connect(m_slideshow_timer, &QTimer::timeout, this, [&]() {
                slideShow();
            });
            m_statusbar->setMsg("Slideshow started", 2);
        } else {
            m_slideshow_files.squeeze();
            /*delete m_slideshow_timer;*/
            /*m_slideshow_timer = nullptr;*/
            disconnect(m_slideshow_timer, 0, 0, 0);
            m_statusbar->setMsg("Stopping Slideshow", 2);
        }
        toggleSlideshow();
    });

    connect(tools__manage_sessions, &QAction::triggered, this, [&]() {
        ManageSessionsDialog *md = new ManageSessionsDialog(m_sessions_dir_path, this);
        connect(md, &ManageSessionsDialog::openSession, this, [&](QString name) { openSession(name); });
        md->open();
    });



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
    connect(m_img_widget, &ImageWidget::fileLoaded, m_statusbar, [&](QString filename) {
        m_statusbar->updateFileInfo(filename);
        if (m_minimap)
            m_minimap->setPixmap(QPixmap(filename));
        if (m_thumbnail_view->currentThumbnail().hasNote())
        {
            // Auto Popup
            if (!m_note_holder->isVisible() && m_auto_notes_popup)
                m_note_holder->setVisible(true);

            m_note_holder->setMarkdown(m_thumbnail_view->currentThumbnail().note());
            m_statusbar->setHasNote(true);
        }
        else
        {
            // Popup close
            if (m_note_holder->isVisible() && m_auto_notes_popup)
                m_note_holder->setVisible(false);
            m_statusbar->setHasNote(false);
        }
    });

    connect(m_img_widget, &ImageWidget::droppedImage, m_thumbnail_view, &ThumbnailView::addThumbnail);

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

    connect(view__minimap, &QAction::triggered, this, [&](bool state) {
        if (state)
        {
            m_img_widget->setMinimapMode(true);
            if (!m_minimap)
            {
                m_minimap = new Minimap();
                m_minimap->setRectColor(m_minimap_rect_color);
                m_minimap->setRectSize(m_minimap_rect_size);
                m_minimap->setPixmap(m_img_widget->getPixmap());
                m_left_pane_layout->addWidget(m_minimap);
                m_minimap->setMainPixmapBoundingRect(m_img_widget->getPixmap().rect());
                connect(m_img_widget, &ImageWidget::getRegion, m_minimap, &Minimap::updateRect);
            }
            else
                connect(m_img_widget, &ImageWidget::getRegion, m_minimap, &Minimap::updateRect);
        }
        else
        {
            m_img_widget->setMinimapMode(false);
            disconnect(m_img_widget, &ImageWidget::getRegion, m_minimap, &Minimap::updateRect);
        }
        m_minimap->setVisible(state);
    });

    connect(tools__pix_analyser, &QAction::triggered, this, [&](bool state) {
        if (state)
        {
            m_img_widget->setPixAnalyseMode(true);
            m_pix_analyser = new PixAnalyser(this);
            connect(m_pix_analyser, &PixAnalyser::visibilityChanged, tools__pix_analyser, [&](bool state) {
                tools__pix_analyser->setChecked(state);
                m_img_widget->setPixAnalyseMode(state);
            });
            m_pix_analyser->setPixmap(m_img_widget->getPixmap());
            connect(m_img_widget, &ImageWidget::mouseMoved, m_pix_analyser, &PixAnalyser::analysePix);
        }
        else {
            m_img_widget->setPixAnalyseMode(false);
            m_pix_analyser->close();
            disconnect(m_img_widget, &ImageWidget::mouseMoved, m_pix_analyser, &PixAnalyser::analysePix);
            disconnect(m_pix_analyser, 0, 0, 0);
            delete m_pix_analyser;
            m_pix_analyser = nullptr;
        }
    });


    connect(view__thumbnails, &QAction::triggered, this, [&](bool state) {
        m_thumbnail_view->setVisible(state);
    });

    connect(view__statusbar, &QAction::triggered, this, [&](bool state) {
        m_statusbar->setVisible(state);
    });

    connect(view__menubar, &QAction::triggered, this, [&](bool state) {
        m_menuBar->setVisible(state);
    });

    connect(view__notes, &QAction::triggered, this, [&](bool state) {
        m_note_holder->setVisible(state);
    });
    
    connect(file__saveSession, &QAction::triggered, this, &IMGV::saveSession);

    connect(view__maximize_image, &QAction::triggered, this, &IMGV::maximizeImage);

    connect(file__exit, &QAction::triggered, this, &QApplication::exit);

    connect(m_thumbnail_view, &ThumbnailView::visibilityChanged, this, [&](bool state) {
        view__thumbnails->setChecked(state);
    });

    connect(m_statusbar, &StatusBar::visibilityChanged, this, [&](bool state) {
        view__statusbar->setChecked(state);
    });

    connect(m_note_holder, &NoteWidget::visibilityChanged, this, [&](bool state) {
        view__notes->setChecked(state);
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
    QShortcut *kb_fit_to_window = new QShortcut(QKeySequence("h"), this);
    QShortcut *kb_toggle_menubar = new QShortcut(QKeySequence("Ctrl+M"), this);
    QShortcut *kb_goto_next = new QShortcut(QKeySequence("j"), this);
    QShortcut *kb_goto_prev = new QShortcut(QKeySequence("k"), this);
    QShortcut *kb_img_maximize = new QShortcut(QKeySequence("Shift+M"), this);
    QShortcut *kb_thumbnail_panel = new QShortcut(QKeySequence("T"), this);
    QShortcut *kb_search_thumbnail = new QShortcut(QKeySequence("/"), this);

    connect(kb_clockwise, &QShortcut::activated, m_img_widget, &ImageWidget::rotateClockwise);
    connect(kb_anticlockwise, &QShortcut::activated, m_img_widget, &ImageWidget::rotateAnticlockwise);
    connect(kb_zoomin, &QShortcut::activated, m_img_widget, &ImageWidget::zoomIn);
    connect(kb_zoomout, &QShortcut::activated, m_img_widget, &ImageWidget::zoomOut);
    connect(kb_zoomreset, &QShortcut::activated, m_img_widget, &ImageWidget::zoomOriginal);
    connect(kb_fliphoriz, &QShortcut::activated, m_img_widget, &ImageWidget::flipHorizontal);
    connect(kb_flipvert, &QShortcut::activated, m_img_widget, &ImageWidget::flipVertical);
    connect(kb_fit_to_width, &QShortcut::activated, m_img_widget, &ImageWidget::fitToWidth);
    connect(kb_fit_to_window, &QShortcut::activated, m_img_widget, &ImageWidget::fitToWindow);
    connect(kb_toggle_menubar, &QShortcut::activated, this, [&]() {
        m_menuBar->setVisible(!m_menuBar->isVisible());
    });

    connect(kb_thumbnail_panel, &QShortcut::activated, this, [&]() {
        m_left_pane->setVisible(!m_left_pane->isVisible());
    });

    connect(kb_goto_next, &QShortcut::activated, m_thumbnail_view, &ThumbnailView::gotoNext);
    connect(kb_goto_prev, &QShortcut::activated, m_thumbnail_view, &ThumbnailView::gotoPrev);
    connect(kb_img_maximize, &QShortcut::activated, this, [&]() {
        maximizeImage();
        view__maximize_image->setChecked(m_image_maximize_mode);
    });

    connect(kb_search_thumbnail, &QShortcut::activated, this, &IMGV::searchThumbnails);

}

void IMGV::initConnections()
{
    connect(m_thumbnail_view, &ThumbnailView::doubleClicked, this, [&](const QModelIndex index) {

        // Don't load the image if already in the current image
        //TODO: /*if (m_thumbnail_view->currentThumbnail().filename() == text) return;*/

        const QString text = index.data(Qt::UserRole).toString();
        m_img_widget->loadFile(text);
        m_note_holder->blockSignals(true);
        m_note_holder->setMarkdown(index.data(Thumbnail::Note).toString());
        m_note_holder->blockSignals(false);
    });
    
    connect(m_note_holder, &NoteWidget::saveRequested, this, [&]() {
        m_thumbnail_view->model()->setNote(m_thumbnail_view->currentIndex(), m_note_holder->toMarkdown());
        m_note_holder->document()->setModified(false);
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
        m_thumbnail_view->createThumbnails(files);
        m_img_widget->loadFile(files[0]);
    }
}

void IMGV::slideShow() noexcept
{
    m_slideshow_index++;
    if (m_slideshow_index >= m_slideshow_files.length())
        m_slideshow_index = 0;
    m_thumbnail_view->setHighlightIndex(m_slideshow_index);
    m_img_widget->loadFile(m_slideshow_files.at(m_slideshow_index));
}

void IMGV::toggleSlideshow() noexcept
{
    if (m_slideshow_timer->isActive())
        m_slideshow_timer->stop();
    else
        m_slideshow_timer->start();
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
        rapidjson::Document document;
        document.SetObject();

        rapidjson::Document::AllocatorType& allocator = document.GetAllocator();

        std::string sess_name = m_session_name.toStdString();
        std::string date = QDateTime::currentDateTime().toString().toStdString();

        document.AddMember("session-name", rapidjson::Value(sess_name.c_str(), allocator), allocator);
        document.AddMember("date", rapidjson::Value(date.c_str(), allocator), allocator);
        
        rapidjson::Value files(rapidjson::kArrayType);
        
        for(unsigned int i=0; i < m_thumbnail_view->count(); i++)
        {
            rapidjson::Value file(rapidjson::kObjectType);
            file.AddMember("path", rapidjson::Value(m_thumbnail_view->item(i, Qt::UserRole).toStdString().c_str(), allocator),
                           allocator);
            file.AddMember("note", rapidjson::Value(m_thumbnail_view->item(i, Thumbnail::Note).toStdString().c_str(), allocator),
                           allocator);
            files.PushBack(file, allocator);
        }
        
        document.AddMember("files", files, allocator);

        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        document.Accept(writer);
        // Save JSON string to file
        auto sess_file = QString("%1%2%3.imgv").arg(m_sessions_dir_path).arg(QDir::separator()).arg(m_session_name);
        std::ofstream ofs(sess_file.toStdString());
        if (ofs.is_open()) {
            ofs << buffer.GetString();
            ofs.close();
        } else {
            qCritical() << "Error opening file for writing.";
        }
        return;
    }

    QStringList sessionFiles = getSessionFiles();
    bool ok;

    QString sessionName = QInputDialog::getText(this, "Save Session", "Please enter a name for the current session", QLineEdit::Normal, "", &ok);
    if (ok && !sessionName.isEmpty())
    {
        if (sessionFiles.indexOf(sessionName + ".imgv") > -1)
        {
            QMessageBox::warning(this, "Overwrite", "A file with that name already exists. Please try again with different name");
            return;
        }

        rapidjson::Document document;
        document.SetObject();

        rapidjson::Document::AllocatorType& allocator = document.GetAllocator();

        std::string sess_name = sessionName.toStdString();
        std::string date = QDateTime::currentDateTime().toString().toStdString();

        document.AddMember("session-name", rapidjson::Value(sess_name.c_str(), allocator), allocator);
        document.AddMember("date", rapidjson::Value(date.c_str(), allocator), allocator);
        
        rapidjson::Value files(rapidjson::kArrayType);
        
        for(unsigned int i=0; i < m_thumbnail_view->count(); i++)
        {
            rapidjson::Value file(rapidjson::kObjectType);
            file.AddMember("path", rapidjson::Value(m_thumbnail_view->item(i, Qt::UserRole).toStdString().c_str(), allocator),
                           allocator);
            file.AddMember("note", rapidjson::Value(m_thumbnail_view->item(i, Thumbnail::Note).toStdString().c_str(), allocator),
                           allocator);
            files.PushBack(file, allocator);
        }
        
        document.AddMember("files", files, allocator);

        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        document.Accept(writer);
        // Save JSON string to file
        auto sess_file = QString("%1%2%3.imgv").arg(m_sessions_dir_path).arg(QDir::separator()).arg(sessionName);
        std::ofstream ofs(sess_file.toStdString());
        if (ofs.is_open()) {
            ofs << buffer.GetString();
            ofs.close();
        } else {
            qCritical() << "Error opening file for writing.";
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
    using namespace rapidjson;
    std::ifstream ifs(filename.toStdString());

    if (!ifs.is_open()) {
        QMessageBox::critical(this, "File not found", "The specified session file was not found");
        return;
    }

    IStreamWrapper isw(ifs);

    Document doc;
    doc.ParseStream(isw);

    if (doc.HasParseError())
    {
        QMessageBox::critical(this, "Error Parsing Session File", "There seems to be a problem reading the session file");
        return;
    }

    if (doc.HasMember("session-name") && doc["session-name"].IsString())
    {
        auto session_name = QString::fromStdString(doc["session-name"].GetString());
        m_session_name = session_name;
        m_statusbar->setSessionName(session_name);
    }
    else QMessageBox::information(this, "Session Info", "No session name found for the current session");

    if (doc.HasMember("date") && doc["date"].IsString())
        m_session_date = QString::fromStdString(doc["date"].GetString());
    else
        QMessageBox::information(this, "Session Info", "No date found for the current session");

    if (doc.HasMember("files") && doc["files"].IsArray())
    {
        const Value& files_arr = doc["files"];
        QList<Thumbnail> files_stringlist;
        for(SizeType i=0; i < files_arr.Size(); i++)
        {
            if (files_arr[i].IsObject())
            {
                auto file = files_arr[i].GetObject();
                if (file.HasMember("path") && file["path"].IsString() && file.HasMember("note") && file["note"].IsString())
                {
                    files_stringlist << Thumbnail(QString::fromStdString(file["path"].GetString()), QString::fromStdString(file["note"].GetString()));
                }
            }
        }
        m_thumbnail_view->createThumbnails(files_stringlist);
        m_img_widget->loadFile(m_thumbnail_view->getFile(0));
    }
    else QMessageBox::information(this, "Session Info", "No files found in the session file");
    ifs.close();
}

void IMGV::parseCommandLineArguments(argparse::ArgumentParser &parser)
{

    if (parser.is_used("--no-config"))
    {
        initDefaultConfig();
    }
    else {
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
                m_thumbnail_view->addThumbnail(QString("%1%2%3").arg(file).arg(QDir::separator()).arg(f));
            return;
        }
        m_img_widget->loadFile(file);

        for(const auto &file: files)
            m_thumbnail_view->addThumbnail(QString::fromStdString(file));
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
                m_thumbnail_view->addThumbnail(QString("%1%2%3").arg(file).arg(QDir::separator()).arg(f));
            return;
        }
        m_img_widget->loadFile(file);

        for(const auto &file: files)
            m_thumbnail_view->addThumbnail(QString::fromStdString(file));
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

void IMGV::maximizeImage()
{
    m_image_maximize_mode = !m_image_maximize_mode;

    m_left_pane->setVisible(!m_image_maximize_mode);
    m_menuBar->setVisible(!m_image_maximize_mode);
    m_statusbar->setVisible(!m_image_maximize_mode);
    if (m_minimap)
        m_minimap->setVisible(!m_image_maximize_mode);
    m_img_widget->setScrollBarsVisibility(!m_image_maximize_mode);
    view__maximize_image->setChecked(m_image_maximize_mode);
}


void IMGV::closeSession()
{
    m_session_name.clear();
    m_thumbnail_view->model()->clear();
    m_img_widget->closeFile();
    m_statusbar->setSessionName("");
    m_statusbar->clearTexts();
    QPixmapCache::clear();
}

void IMGV::openSession(QString &file)
{
    if (m_session_name.isEmpty() && m_thumbnail_view->count() == 0)
    {
        closeSession();
        readSessionFile(file);
    }
    else
    {
        QMessageBox msgBox;
        QAbstractButton *thisWindowBtn = msgBox.addButton("Take over this rindow", QMessageBox::YesRole);
        QAbstractButton *newWindowBtn = msgBox.addButton("New window", QMessageBox::NoRole);
        QAbstractButton *add_to_this_window_btn = msgBox.addButton("Add images to this session", QMessageBox::ApplyRole);
        QAbstractButton *cancel_btn = msgBox.addButton("Cancel Action", QMessageBox::RejectRole);
        msgBox.setText("There is already a session open in this window or there are images opened. What do you want to do?");
        msgBox.setWindowTitle("Open Session");
        msgBox.exec();

        if (msgBox.clickedButton() == thisWindowBtn)
        {
            closeSession();
            readSessionFile(file);
        }
        else if (msgBox.clickedButton() == newWindowBtn)
        {
            openSessionInNewWindow(file);
        }
        else if (msgBox.clickedButton() == add_to_this_window_btn)
        {
            readSessionFile(file);
        }
        else if (msgBox.clickedButton() == cancel_btn)
        {
            return;
        }
    }
}

void IMGV::newSession() noexcept
{
    QStringList sessions = getSessionFiles();
    QString sessionName;
    while (true)
    {
        sessionName = QInputDialog::getText(this, "New Session", "Please enter a name for the session");
        if (sessionName.isEmpty()) return;
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

void IMGV::addNote() noexcept
{
}

void IMGV::ThumbSearchTextChanged(QString text) noexcept
{
    m_thumbnail_view->search(text);
}

void IMGV::toggleNotes() noexcept
{
    m_note_holder->setVisible(!m_note_holder->isVisible());
    if (m_note_holder->isVisible())
        m_note_holder->setFocus();
}

void IMGV::searchThumbnails() noexcept
{
    m_thumbnail_view->searchMode(true);
    m_thumbnail_search_edit->setVisible(true);
    m_thumbnail_search_edit->setFocus();
    m_thumbnail_search_edit->selectAll();
}

void IMGV::toggleThumbnailPanel() noexcept
{
    m_thumbnail_view->setVisible(!m_thumbnail_view->isVisible());
}

void IMGV::toggleStatusbar() noexcept
{
    m_statusbar->setVisible(!m_statusbar->isVisible());
}

void IMGV::toggleMenubar() noexcept
{
    m_menuBar->setVisible(!m_menuBar->isVisible());
}
