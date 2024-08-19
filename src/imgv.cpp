#include "imgv.hpp"


IMGV::IMGV(argparse::ArgumentParser &parser, QWidget *parent)
    : QMainWindow(parent)
{

    // 100 MB cache limit
    setWindowIcon(QIcon(":/resources/images/logo.svg"));

    m_note_holder->setAcceptRichText(true);
    helpMenu->addAction(help__about);

    connect(help__about, &QAction::triggered, [&]() {
        AboutDialog *about = new AboutDialog(this);
        about->open();
    });
    m_note_holder->setVisible(false);

    m_splitter->setHandleWidth(0);

    m_right_pane_layout = new QVBoxLayout();
    m_right_pane->setLayout(m_right_pane_layout);
    m_right_pane_layout->setContentsMargins(0, 0, 0, 0);

    m_right_pane_splitter->addWidget(m_img_widget);
    m_right_pane_splitter->addWidget(m_note_holder);

    m_supported_image_formats = 
        "Images ("
        "*.art *.avi *.avs *.bmp *.cgm *.cin *.cmyk *.cmyka *.cur *.cut "
        "*.dcm *.dcx *.dib *.dng *.dot *.dpx *.emf *.epdf *.epi *.eps *.eps2 "
        "*.eps3 *.epsf *.epsi *.ept *.fax *.fig *.fits *.fpx *.gif *.gplt *.gray "
        "*.hpgl *.html *.ico *.info *.jbig *.jng *.jp2 *.jpc *.jpeg *.jpg *.man "
        "*.mat *.miff *.mono *.mng *.mpeg *.m2v *.mpc *.msl *.mtv *.mvg *.otb "
        "*.p7 *.palm *.pam *.pbm *.pcd *.pcl *.pcx *.pdb *.pdf *.pfa *.pfb *.pgm "
        "*.picon *.pict *.pix *.png *.pnm *.ppm *.ps *.ps2 *.ps3 *.psd *.ptif "
        "*.pwp *.rad *.rgb *.rgba *.rla *.rle *.sct *.sfw *.sgi *.shtml *.sun "
        "*.svg *.tga *.tiff *.tim *.ttf *.txt *.uil *.uyvy *.vicar *.viff *.wbmp "
        "*.wmf *.wpg *.xbm *.xcf *.xpm *.xwd *.ycbcr *.ycbcra *.yuv"
        ")";

    m_supported_image_formats_list = {
        "art", "avi", "avs", "bmp", "cgm", "cin", "cmyk", "cmyka", "cur", "cut",
        "dcm", "dcx", "dib", "dng", "dot", "dpx", "emf", "epdf", "epi", "eps", "eps2",
        "eps3", "epsf", "epsi", "ept", "fax", "fig", "fits", "fpx", "gif", "gplt", "gray",
        "hpgl", "html", "ico", "info", "jbig", "jng", "jp2", "jpc", "jpeg", "jpg", "man",
        "mat", "miff", "mono", "mng", "mpeg", "m2v", "mpc", "msl", "mtv", "mvg", "otb",
        "p7", "palm", "pam", "pbm", "pcd", "pcl", "pcx", "pdb", "pdf", "pfa", "pfb", "pgm",
        "picon", "pict", "pix", "png", "pnm", "ppm", "ps", "ps2", "ps3", "psd", "ptif",
        "pwp", "rad", "rgb", "rgba", "rla", "rle", "sct", "sfw", "sgi", "shtml", "sun",
        "svg", "tga", "tiff", "tim", "ttf", "txt", "uil", "uyvy", "vicar", "viff", "wbmp",
        "wmf", "wpg", "xbm", "xcf", "xpm", "xwd", "ycbcr", "ycbcra", "yuv"
    };

    parseCommandLineArguments(parser);
    initMenu();
    initConnections();
    addSessionsToOpenSessionMenu();

    connect(m_note_holder, &NoteWidget::modificationChanged, m_statusbar, &StatusBar::setNoteModified);
    connect(m_thumbnail_tools_widget, &ThumbnailTools::search, this, &IMGV::searchThumbnails);
    connect(m_thumbnail_tools_widget, &ThumbnailTools::filter, this, &IMGV::filterThumbnails);
    connect(m_thumbnail_tools_widget, &ThumbnailTools::resetFilter, [&]() { m_thumbnail_view->filterMode(false); });

    /*connect(m_note_holder, &NoteWidget::visibilityChanged, m_statusbar, &StatusBar::modificationLabelVisiblity);*/

    m_right_pane_splitter->setStretchFactor(0, 1);
    m_right_pane_layout->addWidget(m_right_pane_splitter);

    m_thumbnail_search_edit->setVisible(false);

    connect(m_img_widget, &ImageWidget::zoomChanged, [&](const qreal &zoom) {
        m_statusbar->setZoom("Zoom: " + QString::number(4 * zoom)); // map zoom from (-50 to 200) to (0 to 500) ish
    });

    connect(m_thumbnail_search_edit, &QLineEdit::textChanged, this, &IMGV::ThumbSearchTextChanged);
    connect(m_thumbnail_search_edit, &QLineEdit::returnPressed, this, [&]() {
        m_thumbnail_view->searchMode(false);
        m_thumbnail_search_edit->setVisible(false);
    });


    m_centralWidget->setLayout(m_layout);
    m_splitter->setStretchFactor(1, 1);
    m_layout->addWidget(m_splitter);
    m_layout->addWidget(m_statusbar);
    setCentralWidget(m_centralWidget);
    m_layout->setContentsMargins(5, 5, 5, 5);
    m_centralWidget->setContentsMargins(0, 0, 0, 0);
    this->setContentsMargins(0, 0, 0, 0);

    connect(m_thumbnail_view, &ThumbnailView::fileChangeRequested, m_img_widget, [&](const QString &filepath) {
        loadFile(filepath);
        m_note_holder->setMarkdown(m_thumbnail_view->currentIndex().data(Thumbnail::Note).toString());
    });

    connect(m_img_widget, &ImageWidget::fileDim, this, [&](const int &w, const int &h) {
        m_statusbar->setImgDimension(w, h);
    });

    if (m_stdin && !isatty(fileno(stdin)))
        processStdin();

    this->show();
}

void IMGV::thumbnailPanel__bottom() noexcept
{

    m_thumbnail_bottom_pane = new QWidget();
    m_splitter->addWidget(m_thumbnail_bottom_pane);
    m_thumbnail_bottom_pane_layout = new QHBoxLayout();
    m_thumbnail_bottom_pane->setLayout(m_thumbnail_bottom_pane_layout);

    m_thumbnail_tools_bottom_layout = new QVBoxLayout();
    m_thumbnail_tools_widget = new ThumbnailTools(Qt::Orientation::Vertical);
    m_thumbnail_tools_bottom_layout->addWidget(m_thumbnail_tools_widget);
    m_thumbnail_tools_bottom_layout->addWidget(m_thumbnail_search_edit);

    m_thumbnail_bottom_pane_layout->addLayout(m_thumbnail_tools_bottom_layout);
    m_thumbnail_bottom_pane_layout->addWidget(m_thumbnail_view);

    m_thumbnail_bottom_pane->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    m_thumbnail_tools_widget->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    m_right_pane->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

    m_thumbnail_view->setFlow(QListView::Flow::LeftToRight);
    m_splitter->setOrientation(Qt::Orientation::Vertical);
    m_splitter->addWidget(m_right_pane);
    m_splitter->addWidget(m_thumbnail_bottom_pane);

}

void IMGV::thumbnailPanel__left() noexcept
{
    m_thumbnail_left_pane = new QWidget();
    m_thumbnail_left_pane_layout = new QVBoxLayout();
    m_thumbnail_left_pane->setLayout(m_thumbnail_left_pane_layout);
    m_thumbnail_tools_widget = new ThumbnailTools(Qt::Orientation::Horizontal);
    m_thumbnail_left_pane_layout->addWidget(m_thumbnail_tools_widget);
    m_thumbnail_left_pane_layout->addWidget(m_thumbnail_search_edit);
    m_thumbnail_left_pane_layout->addWidget(m_thumbnail_view);
    m_splitter->addWidget(m_thumbnail_left_pane);
    m_splitter->addWidget(m_right_pane);

}

void IMGV::thumbnailPanel__right() noexcept
{

    m_thumbnail_left_pane = new QWidget();
    m_thumbnail_left_pane_layout = new QVBoxLayout();
    m_thumbnail_left_pane->setLayout(m_thumbnail_left_pane_layout);
    m_thumbnail_tools_widget = new ThumbnailTools(Qt::Orientation::Horizontal);
    m_thumbnail_left_pane_layout->addWidget(m_thumbnail_tools_widget);
    m_thumbnail_left_pane_layout->addWidget(m_thumbnail_search_edit);
    m_thumbnail_left_pane_layout->addWidget(m_thumbnail_view);
    m_splitter->addWidget(m_right_pane);
    m_splitter->addWidget(m_thumbnail_left_pane);
}

void IMGV::thumbnailPanel__top() noexcept
{

    m_thumbnail_bottom_pane = new QWidget();
    m_splitter->addWidget(m_thumbnail_bottom_pane);
    m_thumbnail_bottom_pane_layout = new QHBoxLayout();
    m_thumbnail_bottom_pane->setLayout(m_thumbnail_bottom_pane_layout);

    m_thumbnail_tools_bottom_layout = new QVBoxLayout();
    m_thumbnail_tools_widget = new ThumbnailTools(Qt::Orientation::Vertical);
    m_thumbnail_tools_bottom_layout->addWidget(m_thumbnail_tools_widget);
    m_thumbnail_tools_bottom_layout->addWidget(m_thumbnail_search_edit);

    m_thumbnail_bottom_pane_layout->addLayout(m_thumbnail_tools_bottom_layout);
    m_thumbnail_bottom_pane_layout->addWidget(m_thumbnail_view);


    m_thumbnail_view->setFlow(QListView::Flow::LeftToRight);
    m_splitter->setOrientation(Qt::Orientation::Vertical);

    m_splitter->addWidget(m_thumbnail_bottom_pane);
    m_splitter->addWidget(m_right_pane);

}

void IMGV::initDefaultConfig() noexcept
{
    m_statusbar->setSpacing(10);
    m_statusbar->addWidgets(QStringList() << "message" << "path" << "note-indicator" << "note-modified-indicator" << "stretch" << "size" << "dimension" << "session");
    initKeybinds();
}

void IMGV::initConfigDirectory() noexcept
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

        // set pixmap cache
        QPixmapCache::setCacheLimit(defaults_table["image_cache"].get_or(10240));

        auto fit_on_load = defaults_table["fit_on_load"].get_or(false);

        if (fit_on_load)
        {
            auto fit_on_load_mode = defaults_table["fit_on_load_mode"].get_or<std::string>("none");

            if (fit_on_load_mode == "height")
            {
                m_img_widget->setFitImageOnLoad(true, ImageWidget::FitOnLoad::FitToHeight);
                fit_on_load__height->setChecked(true);
            }
            else if (fit_on_load_mode == "width")
            {
                m_img_widget->setFitImageOnLoad(true, ImageWidget::FitOnLoad::FitToWidth);
                fit_on_load__width->setChecked(true);
            }
            else
            {
                fit_on_load__height->setChecked(true);
                m_img_widget->setFitImageOnLoad(true, ImageWidget::FitOnLoad::FitToHeight);
            }
        }
        else 
        {
            m_img_widget->setFitImageOnLoad(false);
            fit_on_load__none->setChecked(true);
        }

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

            // thumbnail layout
            
            auto layout = thumbnails_table["layout"].get_or<std::string>("left");

            if (layout == "left")
                thumbnailPanel__left();
            else if (layout == "bottom")
                thumbnailPanel__bottom();
            else if (layout == "right")
                thumbnailPanel__right();
            else if (layout == "top")
                thumbnailPanel__top();

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

                // TODO:
                /*m_thumbnail_view->setIconSize(QSize(icon_size_table["width"].get_or(100), icon_size_table["height"].get_or(100)));  // Set the size for thumbnails*/
            }

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

            sol::optional<sol::table> text_table_optional = thumbnails_table["text"];

            if (text_table_optional)
            {
                sol::table text_table = text_table_optional.value();

                auto elide = text_table["elide"].get_or<std::string>("middle");

                if (elide == "none")
                    m_thumbnail_view->setTextElideMode(Qt::ElideNone);

                else if (elide == "right")
                    m_thumbnail_view->setTextElideMode(Qt::ElideRight);

                else if (elide == "left")
                    m_thumbnail_view->setTextElideMode(Qt::ElideLeft);

                else if (elide == "middle")
                    m_thumbnail_view->setTextElideMode(Qt::ElideMiddle);

                else
                    m_thumbnail_view->setTextElideMode(Qt::ElideNone);

                auto wrap = text_table["wrap"].get_or(true);

                if (wrap)
                    m_thumbnail_view->setWordWrap(wrap);

            }


            auto alternate_colors = thumbnails_table["alternate_colors"].get_or(false);

            if (alternate_colors)
            {
                m_thumbnail_view->setAlternatingRowColors(true);
                qDebug() << "DD";
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
                    connect(shortcut, &QShortcut::activated, m_img_widget, &ImageWidget::zoomIn);

                } else if (action == "zoom_out") {
                    connect(shortcut, &QShortcut::activated, m_img_widget, &ImageWidget::zoomOut);

                } else if (action == "zoom_reset") {
                    connect(shortcut, &QShortcut::activated, m_img_widget, &ImageWidget::zoomOriginal);

                } else if (action == "rotate_clockwise") {
                    connect(shortcut, &QShortcut::activated, m_img_widget, &ImageWidget::rotateClockwise);

                } else if (action == "rotate_anticlockwise") {
                    connect(shortcut, &QShortcut::activated, m_img_widget, &ImageWidget::rotateAnticlockwise);

                } else if (action == "flip_vertical") {
                    connect(shortcut, &QShortcut::activated, m_img_widget, &ImageWidget::flipVertical);

                } else if (action == "flip_horizontal") {
                    connect(shortcut, &QShortcut::activated, m_img_widget, &ImageWidget::flipHorizontal);

                } else if (action == "left") {
                    connect(shortcut, &QShortcut::activated, m_img_widget, &ImageWidget::moveLeft);

                } else if (action == "down") {
                    connect(shortcut, &QShortcut::activated, m_img_widget, &ImageWidget::moveDown);

                } else if (action == "up") {
                    connect(shortcut, &QShortcut::activated, m_img_widget, &ImageWidget::moveUp);

                } else if (action == "right") {
                    connect(shortcut, &QShortcut::activated, m_img_widget, &ImageWidget::moveRight);

                } else if (action == "next") {
                    connect(shortcut, &QShortcut::activated, m_thumbnail_view, &ThumbnailView::gotoNext);

                } else if (action == "prev") {
                    connect(shortcut, &QShortcut::activated, m_thumbnail_view, &ThumbnailView::gotoPrev);

                } else if (action == "notes") {
                    connect(shortcut, &QShortcut::activated, this, &IMGV::toggleNotes);

                } else if (action == "maximize") {
                    connect(shortcut, &QShortcut::activated, this, &IMGV::maximizeImage);

                } else if (action == "search") {
                    connect(shortcut, &QShortcut::activated, this, &IMGV::searchThumbnails);

                } else if (action == "toggle_menubar") {
                    connect(shortcut, &QShortcut::activated, this, &IMGV::toggleMenubar);

                } else if (action == "toggle_statusbar") {
                    connect(shortcut, &QShortcut::activated, this, &IMGV::toggleStatusbar);

                } else if (action == "toggle_thumbnail_panel") {
                    connect(shortcut, &QShortcut::activated, this, &IMGV::toggleThumbnailPanel);

                } else if (action == "fit_width") {
                    connect(shortcut, &QShortcut::activated, m_img_widget, &ImageWidget::fitToWidth);

                } else if (action == "fit_height") {
                    connect(shortcut, &QShortcut::activated, m_img_widget, &ImageWidget::fitToHeight);
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

            sol::optional<sol::table> rect_table_optional = minimap_table["rect"];

            if (rect_table_optional)
            {
                sol::table rect_table = rect_table_optional.value();

                m_img_widget->setMinimapRectFillColor(QString::fromStdString(rect_table["color"].get_or<std::string>("")));
                m_img_widget->setMinimapRectColor(QString::fromStdString(rect_table["outline"].get_or<std::string>("#000000")));
                m_img_widget->setMinimapRectAlpha(rect_table["alpha"].get_or(0.5));
            }

            auto show = minimap_table["show"].get_or(false);
            view__minimap->setChecked(show);
            m_img_widget->setMinimapMode(show);
            m_img_widget->setMinimapAutoHide(minimap_table["auto_hide"].get_or(false));

            auto location = minimap_table["location"].get_or<std::string>("bottom-right");

            if (location == "bottom-right")
                m_img_widget->setMinimapLocation(Minimap::Location::BottomRight);

            else if (location == "bottom-left")
                m_img_widget->setMinimapLocation(Minimap::Location::BottomLeft);

            else if (location == "top-left")
                m_img_widget->setMinimapLocation(Minimap::Location::TopLeft);

            else if (location == "top-right")
                m_img_widget->setMinimapLocation(Minimap::Location::TopRight);

            else
                m_img_widget->setMinimapLocation(Minimap::Location::BottomRight);

            sol::optional<sol::table> minimap_size_table_optional = minimap_table["size"];
            if (minimap_size_table_optional)
            {
                auto minimap_size_table = minimap_size_table_optional.value();

                m_img_widget->setMinimapSize(QSize(minimap_size_table["width"].get_or(100), minimap_size_table["height"].get_or(100)));  // Set the size for thumbnail
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

void IMGV::initMenu() noexcept
{

    setMenuBar(m_menuBar);

    fileMenu->addAction(file__openAction);
    fileMenu->addAction(file__openNewWindowAction);
    fileMenu->addAction(file__exit);

    sessionMenu->addMenu(session__openSession);
    sessionMenu->addAction(session__newSession);
    sessionMenu->addAction(session__saveSession);
    sessionMenu->addAction(session__closeSession);
    sessionMenu->addMenu(session__tags);
    sessionMenu->addAction(session__manage_sessions);

    session__tags->addAction(tags_assign);
    session__tags->addAction(tags_new);
    session__tags->addAction(tags_manage);

    tags_manage->setCheckable(true);

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
    editMenu->addMenu(edit__fit);
    editMenu->addMenu(edit__fit_on_load);

    edit__fit->addAction(fit__width);
    edit__fit->addAction(fit__height);

    edit__fit_on_load->addAction(fit_on_load__width);
    edit__fit_on_load->addAction(fit_on_load__height);
    edit__fit_on_load->addAction(fit_on_load__none);

    fit_on_load_action_group->addAction(fit_on_load__width);
    fit_on_load_action_group->addAction(fit_on_load__height);
    fit_on_load_action_group->addAction(fit_on_load__none);

    fit_on_load__width->setCheckable(true);
    fit_on_load__height->setCheckable(true);
    fit_on_load__none->setCheckable(true);

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

    toolsMenu->addAction(tools__pix_analyser);
    toolsMenu->addAction(tools__slideshow);

    tools__pix_analyser->setCheckable(true);
    tools__slideshow->setCheckable(true);

    connect(fit_on_load__width, &QAction::triggered, m_img_widget, [&](const bool &state) {
        if (state)
            m_img_widget->setFitImageOnLoad(true, ImageWidget::FitOnLoad::FitToWidth);
    });

    connect(fit_on_load__height, &QAction::triggered, m_img_widget, [&](const bool &state) {
        if (state)
            m_img_widget->setFitImageOnLoad(true, ImageWidget::FitOnLoad::FitToHeight);
    });

    connect(fit_on_load__none, &QAction::triggered, m_img_widget, [&](const bool &state) {
        if (state)
            m_img_widget->setFitImageOnLoad(false);
    });

    connect(fit__width, &QAction::triggered, m_img_widget, &ImageWidget::fitToWidth);
    connect(fit__height, &QAction::triggered, m_img_widget, &ImageWidget::fitToHeight);

    connect(tags_assign, &QAction::triggered, this, &IMGV::assignTagToImage);
    connect(tags_new, &QAction::triggered, this, &IMGV::createTag);
    connect(tags_manage, &QAction::triggered, this, &IMGV::manageTags);

    connect(zoom__in, &QAction::triggered, m_img_widget, &ImageWidget::zoomIn);
    connect(zoom__out, &QAction::triggered, m_img_widget, &ImageWidget::zoomOut);
    connect(zoom__reset, &QAction::triggered, m_img_widget, &ImageWidget::zoomOriginal);

    connect(tools__slideshow, &QAction::triggered, this, [&](const bool &state) {
        if (state)
        {
            if (!m_slideshow_timer)
            {
                m_slideshow_timer = new QTimer();
                m_slideshow_timer->setInterval(2 * 1000);
                connect(m_slideshow_timer, &QTimer::timeout, this, [&]() {
                    slideShow();
                });
            }

            // TODO: Don't start it from the beginning each time
            m_slideshow_index = -1;
            m_slideshow_files = m_thumbnail_view->getAllFiles();
            setMsg("Slideshow started", 2);
        } else {
            /*delete m_slideshow_timer;*/
            /*m_slideshow_timer = nullptr;*/
            disconnect(m_slideshow_timer);
            setMsg("Stopping Slideshow", 2);
        }
        toggleSlideshow();
    });

    session__closeSession->setEnabled(false);

    connect(session__newSession, &QAction::triggered, this, &IMGV::newSession);
    connect(session__closeSession, &QAction::triggered, this, &IMGV::closeSession);


    connect(session__saveSession, &QAction::triggered, this, &IMGV::saveSession);

    connect(session__manage_sessions, &QAction::triggered, [&]() {
        ManageSessionsDialog *md = new ManageSessionsDialog(m_sessions_dir_path, this);
        connect(md, &ManageSessionsDialog::openSession, [&](QString name) { openSession(name); });
        md->open();
    });

    connect(file__openAction, &QAction::triggered, this, &IMGV::openImage);
    connect(file__openNewWindowAction, &QAction::triggered, this, &IMGV::openImageInNewWindow);
    connect(m_img_widget, &ImageWidget::fileLoaded, m_statusbar, [&](const QString &filename) {
        m_statusbar->updateFileInfo(filename);
        if (m_thumbnail_view->currentThumbnail().hasNote())
        {
            // Auto Popup
            if (m_auto_notes_popup && !m_note_holder->isVisible())
                m_note_holder->setVisible(true);

            m_note_holder->setMarkdown(m_thumbnail_view->currentThumbnail().note());
            m_statusbar->setHasNote(true);
        }
        else
        {
            // Popup close
            if (m_auto_notes_popup && m_note_holder->isVisible())
                m_note_holder->setVisible(false);
            m_statusbar->setHasNote(false);
        }
    });

    connect(m_img_widget, &ImageWidget::droppedImage, [&](const QString& file) {
        m_thumbnail_view->createThumbnail(file);
    });


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

    connect(view__minimap, &QAction::triggered, this, [&](const bool &state) {
        m_img_widget->setMinimapMode(state);
    });

    connect(tools__pix_analyser, &QAction::triggered, m_img_widget, [&](const bool &state) {
        if (state)
            setMsg("Click to save picked color", -1);
        m_img_widget->setPixAnalyseMode(state);

    });

    connect(m_img_widget, &ImageWidget::pixAnalyserVisibilityChanged, [&](const bool &state) {
        tools__pix_analyser->setChecked(state);
        if (!state)
            m_statusbar->hideMsg();
    });

    connect(m_img_widget, &ImageWidget::minimapVisibilityChanged, view__minimap, &QAction::setChecked);

    connect(view__thumbnails, &QAction::triggered, this, [&](const bool &state) {
        m_thumbnail_view->setVisible(state);
    });

    connect(view__statusbar, &QAction::triggered, this, [&](const bool &state) {
        m_statusbar->setVisible(state);
    });

    connect(view__menubar, &QAction::triggered, this, [&](const bool &state) {
        m_menuBar->setVisible(state);
    });

    connect(view__notes, &QAction::triggered, this, [&](const bool &state) {
        m_note_holder->setVisible(state);
    });
    

    connect(view__maximize_image, &QAction::triggered, this, &IMGV::maximizeImage);

    connect(file__exit, &QAction::triggered, this, &QApplication::exit);

    connect(m_thumbnail_view, &ThumbnailView::visibilityChanged, [&](const bool &state) {
        view__thumbnails->setChecked(state);
    });

    connect(m_statusbar, &StatusBar::visibilityChanged, [&](const bool &state) {
        view__statusbar->setChecked(state);
    });

    connect(m_note_holder, &NoteWidget::visibilityChanged, [&](const bool &state) {
        view__notes->setChecked(state);
    });

}

void IMGV::initKeybinds() noexcept
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
    QShortcut *kb_search_thumbnail = new QShortcut(QKeySequence("/"), this);

    connect(kb_clockwise, &QShortcut::activated, m_img_widget, &ImageWidget::rotateClockwise);
    connect(kb_anticlockwise, &QShortcut::activated, m_img_widget, &ImageWidget::rotateAnticlockwise);
    connect(kb_zoomin, &QShortcut::activated, m_img_widget, &ImageWidget::zoomIn);
    connect(kb_zoomout, &QShortcut::activated, m_img_widget, &ImageWidget::zoomOut);
    connect(kb_zoomreset, &QShortcut::activated, m_img_widget, &ImageWidget::zoomOriginal);
    connect(kb_fliphoriz, &QShortcut::activated, m_img_widget, &ImageWidget::flipHorizontal);
    connect(kb_flipvert, &QShortcut::activated, m_img_widget, &ImageWidget::flipVertical);
    connect(kb_fit_to_width, &QShortcut::activated, m_img_widget, &ImageWidget::fitToWidth);
    connect(kb_fit_to_height, &QShortcut::activated, m_img_widget, &ImageWidget::fitToHeight);

    connect(kb_toggle_menubar, &QShortcut::activated, [&]() {
        m_menuBar->setVisible(!m_menuBar->isVisible());
    });

    /*connect(kb_thumbnail_panel, &QShortcut::activated, [&]() {*/
    /*    m_left_pane->setVisible(!m_left_pane->isVisible());*/
    /*});*/

    connect(kb_goto_next, &QShortcut::activated, m_thumbnail_view, &ThumbnailView::gotoNext);
    connect(kb_goto_prev, &QShortcut::activated, m_thumbnail_view, &ThumbnailView::gotoPrev);
    connect(kb_search_thumbnail, &QShortcut::activated, this, &IMGV::searchThumbnails);

    connect(kb_img_maximize, &QShortcut::activated, [&]() {
        maximizeImage();
        view__maximize_image->setChecked(m_image_maximize_mode);
    });


}

void IMGV::initConnections() noexcept
{
    connect(m_thumbnail_view, &ThumbnailView::doubleClicked, [this](const QModelIndex &index) {

        // Don't load the image if already in the current image
        //TODO: /*if (m_thumbnail_view->currentThumbnail().filename() == text) return;*/

        const QString text = index.data(Qt::UserRole).toString();
        loadFile(text);
        m_note_holder->blockSignals(true); // FIX : is this necessary ?
        m_note_holder->setMarkdown(index.data(Thumbnail::Note).toString());
        m_note_holder->blockSignals(false);
    });
    
    connect(m_note_holder, &NoteWidget::saveRequested, [&]() {
        m_thumbnail_view->model()->setNote(m_thumbnail_view->currentIndex(), m_note_holder->toMarkdown());
        m_note_holder->document()->setModified(false);
    });
}

void IMGV::openImageInNewWindow() noexcept
{
    QStringList files = QFileDialog::getOpenFileNames(this, tr("Open Image"), "", m_supported_image_formats);
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

void IMGV::openImage() noexcept
{
    QStringList files = QFileDialog::getOpenFileNames(this, tr("Open Image"), "", m_supported_image_formats);
    if (!files.isEmpty()) {
        m_thumbnail_view->createThumbnails(files);
        loadFile(files.at(0));
    }
}

void IMGV::slideShow() noexcept
{
    m_slideshow_index++;
    if (m_slideshow_index >= m_slideshow_files.length())
        m_slideshow_index = 0;
    m_thumbnail_view->setHighlightIndex(m_slideshow_index);
    loadFile(m_slideshow_files.at(m_slideshow_index));
}

void IMGV::toggleSlideshow() const noexcept
{
    if (m_slideshow_timer->isActive())
    {
        m_slideshow_timer->stop();
        setMsg("Slideshow stopped", 5);
    }
    else
    {
        m_slideshow_timer->start();
        setMsg("Slideshow resumed", 5);
    }
}

QStringList IMGV::getSessionFiles() const noexcept
{
    return QDir(m_sessions_dir_path).entryList(QStringList() << "*.imgv" << "*.IMGV", QDir::Files);
}

void IMGV::saveSession() noexcept
{

    if (!m_session_name.isEmpty())
    {
        rapidjson::Document document;
        document.SetObject();

        rapidjson::Document::AllocatorType& allocator = document.GetAllocator();

        std::string sess_name = m_session_name.toStdString();
        std::string date = QDateTime::currentDateTime().toString().toStdString();

        document.AddMember("session-name", rapidjson::Value(sess_name.c_str(), allocator), allocator);
        document.AddMember("date", rapidjson::Value(date.c_str(), allocator), allocator);

        // tags
        rapidjson::Value tags(rapidjson::kArrayType);
        for (unsigned int i=0; i < m_tags.size(); i++)
            tags.PushBack(rapidjson::Value(m_tags.at(i).toStdString().c_str(), allocator), allocator);

        document.AddMember("tags", tags, allocator);
        
        // files
        rapidjson::Value files(rapidjson::kArrayType);
        
        for(unsigned int i=0; i < m_thumbnail_view->count(); i++)
        {
            rapidjson::Value file(rapidjson::kObjectType);

            file.AddMember("path", rapidjson::Value(m_thumbnail_view->item(i, Qt::UserRole).toStdString().c_str(), allocator),
                           allocator);

            file.AddMember("note", rapidjson::Value(m_thumbnail_view->item(i, Thumbnail::Note).toStdString().c_str(), allocator),
                           allocator);

            file.AddMember("tag", rapidjson::Value(m_thumbnail_view->item(i, Thumbnail::Tag).toStdString().c_str(), allocator),
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
        setMsg("Session Saved", 2);
        return;
    }

    QStringList sessionFiles = getSessionFiles();
    bool ok;

    QString sessionName = QInputDialog::getText(this, "Save Session", "Please enter a name for the current session", QLineEdit::Normal, "", &ok);
    if (ok)
    {
        if (!sessionName.isEmpty())
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

            // tags
            rapidjson::Value tags(rapidjson::kArrayType);
            for (int i=0; i < m_tags.size(); i++)
                tags.PushBack(rapidjson::Value(m_tags.at(i).toStdString().c_str(), allocator), allocator);
            document.AddMember("tags", tags, allocator);


            // Files
            rapidjson::Value files(rapidjson::kArrayType);

            for(unsigned int i=0; i < m_thumbnail_view->count(); i++)
            {
                rapidjson::Value file(rapidjson::kObjectType);

                file.AddMember("path", rapidjson::Value(m_thumbnail_view->item(i, Qt::UserRole).toStdString().c_str(), allocator),
                               allocator);

                file.AddMember("note", rapidjson::Value(m_thumbnail_view->item(i, Thumbnail::Note).toStdString().c_str(), allocator),
                               allocator);

                file.AddMember("tag", rapidjson::Value(m_thumbnail_view->item(i, Thumbnail::Tag).toStdString().c_str(), allocator),
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
            return;
        }
    }
    else return;

    addSessionToOpenSessionMenu(m_session_name + ".imgv");
}

void IMGV::addSessionToOpenSessionMenu(const QString &sessionName) noexcept
{
    QAction *action = new QAction(sessionName);
    connect(action, &QAction::triggered, this, [&]() {
        auto filename = reinterpret_cast<QAction*>(sender())->text();
        QString file = QString("%1%2%3").arg(m_sessions_dir_path).arg(QDir::separator()).arg(filename);
        openSession(file);
    });
    session__openSession->addAction(action);
}

void IMGV::readSessionFile(QString& filename) noexcept
{
    using namespace rapidjson;

    if (!filename.endsWith(".imgv"))
        filename = filename + ".imgv";



    auto sessions = getSessionFiles();
    if (sessions.indexOf(filename)> -1)
        filename = m_sessions_dir_path + QDir::separator() + filename;

    std::ifstream ifs(filename.toStdString());

    if (!ifs.is_open()) {
        QMessageBox::critical(this, "File not found", "The specified session file was not found");
        return;
    }

    IStreamWrapper isw(ifs);

    Document doc;
    doc.ParseStream(isw);
    if (doc.HasParseError()) {
        QMessageBox::critical(this, "Error Parsing Session File", "There seems to be a problem reading the session file");
        return;
    }

    // Read Session Name
    if (doc.HasMember("session-name") && doc["session-name"].IsString())
    {
        auto session_name = QString::fromStdString(doc["session-name"].GetString());
        m_session_name = session_name;
        m_statusbar->setSessionName(session_name);
    }
    else QMessageBox::information(this, "Session Info", "No session name found for the current session");

    // Read Modified Date
    if (doc.HasMember("date") && doc["date"].IsString())
        m_session_date = QString::fromStdString(doc["date"].GetString());
    else
        QMessageBox::information(this, "Session Info", "No date found for the current session");

    // Read Tags
    if (doc.HasMember("tags") && doc["tags"].IsArray())
    {
        const Value &tags_arr = doc["tags"];
        m_tags.reserve(tags_arr.Size());
        for(SizeType i=0; i < tags_arr.Size(); i++)
            m_tags[i] = QString::fromStdString(tags_arr[i].GetString());
    }

    // Read Files
    if (doc.HasMember("files") && doc["files"].IsArray())
    {
        const Value& files_arr = doc["files"];
        if (files_arr.Size() == 0) return;
        QList<Thumbnail> files_stringlist;
        files_stringlist.reserve(files_arr.Size());
        for(SizeType i=0; i < files_arr.Size(); i++)
        {
            if (files_arr[i].IsObject())
            {
                auto file = files_arr[i].GetObject();

                if ((file.HasMember("path") && file["path"].IsString()) &&
                    (file.HasMember("note") && file["note"].IsString()) &&
                    (file.HasMember("tag") && file["tag"].IsString()))
                {
                    files_stringlist.emplace_back(QString::fromStdString(file["path"].GetString()),
                                                  QString::fromStdString(file["note"].GetString()),
                                                  QString::fromStdString(file["tag"].GetString()));
                }
            }
        }
        m_thumbnail_view->createThumbnails(files_stringlist);
        loadFile(m_thumbnail_view->getFile(0));

    }
    else
        QMessageBox::information(this, "Session Info", "No files found in the session file");
    ifs.close();

}

void IMGV::readSessionFile(QString&& filename) noexcept
{
    using namespace rapidjson;

    if (!filename.endsWith(".imgv"))
        filename = filename + ".imgv";

    auto sessions = getSessionFiles();
    if (sessions.indexOf(filename)> -1)
        filename = m_sessions_dir_path + QDir::separator() + filename;

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

    // Read Session Name
    if (doc.HasMember("session-name") && doc["session-name"].IsString())
    {
        auto session_name = QString::fromStdString(doc["session-name"].GetString());
        m_session_name = session_name;
        m_statusbar->setSessionName(session_name);
    }
    else QMessageBox::information(this, "Session Info", "No session name found for the current session");

    // Read Modified Date
    if (doc.HasMember("date") && doc["date"].IsString())
        m_session_date = QString::fromStdString(doc["date"].GetString());
    else
        QMessageBox::information(this, "Session Info", "No date found for the current session");

    // Read Tags
    if (doc.HasMember("tags") && doc["tags"].IsArray())
    {
        const Value &tags_arr = doc["tags"];
        m_tags.reserve(tags_arr.Size());
        for(SizeType i=0; i < tags_arr.Size(); i++)
            m_tags[i] = QString::fromStdString(tags_arr[i].GetString());
    }

    // Read Files
    if (doc.HasMember("files") && doc["files"].IsArray())
    {
        const Value& files_arr = doc["files"];
        QList<Thumbnail> files_stringlist;
        files_stringlist.reserve(files_arr.Size());
        for(SizeType i=0; i < files_arr.Size(); i++)
        {
            if (files_arr[i].IsObject())
            {
                auto file = files_arr[i].GetObject();

                if ((file.HasMember("path") && file["path"].IsString()) &&
                    (file.HasMember("note") && file["note"].IsString()) &&
                    (file.HasMember("tag") && file["tag"].IsString()))
                {
                    files_stringlist.emplace_back(QString::fromStdString(file["path"].GetString()),
                                                  QString::fromStdString(file["note"].GetString()),
                                                  QString::fromStdString(file["tag"].GetString()));
                }
            }
        }
        m_thumbnail_view->createThumbnails(files_stringlist);
        loadFile(m_thumbnail_view->getFile(0));
    }
    else
        QMessageBox::information(this, "Session Info", "No files found in the session file");
    ifs.close();
}

void IMGV::parseCommandLineArguments(const argparse::ArgumentParser &parser) noexcept
{

    if (parser.is_used("--no-config"))
        initDefaultConfig();
    else
        initConfigDirectory();

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
        setMsg("Session file is not saved. Please do it manually", 5);
        m_statusbar->setSessionName(ses_name);
        /*saveSession();*/
    }

    if (parser.present("--session"))
    {
        readSessionFile(QString::fromStdString(parser.get<std::string>("--session")));
    }

    if (parser.is_used("--input"))
    {
        std::vector<std::string> files = parser.get<std::vector<std::string>>("--input");
        QString file = QString::fromStdString(files[0]);

        if (QFileInfo(file).isDir())
        {
            QStringList dirfiles = QDir(file).entryList(m_supported_image_formats_list, QDir::Files);
            for(int i=0; i < dirfiles.size(); i++)
                dirfiles[i] = QString("%1%2%3").arg(file).arg(QDir::separator()).arg(dirfiles.at(i));

            m_thumbnail_view->createThumbnails(dirfiles);
            return;
        }
        loadFile(file);

        QStringList dd;
        dd.reserve(files.size());

        for (const auto &file : files)
            dd.push_back(QString::fromStdString(file));

        m_thumbnail_view->createThumbnails(dd);
    }


    if (parser.is_used("-"))
    {
        m_stdin = true;
    }

    if (parser.is_used("files"))
    {
        std::vector<std::string> files = parser.get<std::vector<std::string>>("files");
        QString file = QString::fromStdString(files.at(0));

        // If directory is mentioned, try to read all the compatible files from the directory
        if (QFileInfo(file).isDir())
        {
            m_thumbnail_view->createThumbnails(QDir(file).entryList(m_supported_image_formats_list, QDir::Files));
            /*for(const auto &f: dirfiles)*/
            /*    m_thumbnail_view->addThumbnail(QString("%1%2%3").arg(file).arg(QDir::separator()).arg(f));*/
            /*return;*/
        }
        else {
            QStringList _files;
            for(const auto &file : files)
                _files << QString::fromStdString(file);
            m_thumbnail_view->createThumbnails(_files);
            loadFile(_files.at(0));
        }
    }

}

void IMGV::openSessionInNewWindow(const QString &file) noexcept
{
    QString program = QCoreApplication::applicationFilePath();

    QStringList arguments;
    arguments << "-s" << file;

    QProcess *process = new QProcess(this);
    process->startDetached();
    process->start(program, arguments);

    if (!process->waitForStarted())
        qDebug() << "FAILED";
}

void IMGV::maximizeImage() noexcept
{
    m_image_maximize_mode = !m_image_maximize_mode;
    /*m_left_pane->setVisible(!m_image_maximize_mode);*/
    m_menuBar->setVisible(!m_image_maximize_mode);
    m_statusbar->setVisible(!m_image_maximize_mode);
    m_img_widget->setScrollBarsVisibility(!m_image_maximize_mode);
    view__maximize_image->setChecked(m_image_maximize_mode);
}


void IMGV::closeSession() noexcept
{
    m_session_name.clear();

    if (!m_tags.isEmpty())
        m_tags.squeeze();

    if (!m_slideshow_files.isEmpty())
        m_slideshow_files.squeeze();

    if (!m_temp_files.isEmpty())
        m_temp_files.squeeze();

    m_note_holder->clear();
    m_note_holder->hide();
    m_thumbnail_view->model()->clear();
    m_img_widget->closeFile();
    m_statusbar->setSessionName("");
    m_statusbar->clearTexts();
    QPixmapCache::clear();
}

void IMGV::openSession(QString &file) noexcept
{
    if (m_session_name.isEmpty() && m_thumbnail_view->count() == 0)
    {
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
            m_tags.squeeze();
            readSessionFile(file);
        }

        else if (msgBox.clickedButton() == newWindowBtn)
            openSessionInNewWindow(file);

        else if (msgBox.clickedButton() == add_to_this_window_btn)
            readSessionFile(file);

        else if (msgBox.clickedButton() == cancel_btn)
            return;
    }
    setMsg(QString("Session %1 opened").arg(m_session_name));
    session__closeSession->setEnabled(true);
}

void IMGV::newSession() noexcept
{
    QStringList sessions = getSessionFiles();
    QString sessionName;

    sessionName = QInputDialog::getText(this, "New Session", "Please enter a name for the session");

    if (sessionName.isEmpty())
        return;

    if (sessions.indexOf(sessionName + ".imgv") > -1)
    {
        QMessageBox::critical(this, "Error creating session", QString("Session with the name %1 already exists. Try naming the name session with some other name").arg(sessionName));
        return;
    }

    QString program = QCoreApplication::applicationFilePath();

    QStringList arguments;
    arguments << "-S" << sessionName;

    QProcess *process = new QProcess(this);
    process->startDetached();
    process->start(program, arguments);

    if (!process->waitForStarted())
        qDebug() << "FAILED";
    else {
        qDebug() << "STARTED DETACHED PROCESS";
    }

    session__closeSession->setEnabled(true);
}

// TODO: addNote() function
void IMGV::addNote() noexcept
{
}

void IMGV::toggleNotes() const noexcept
{
    m_note_holder->setVisible(!m_note_holder->isVisible());
    /*if (m_note_holder->isVisible())*/
    /*    m_note_holder->setFocus();*/
}

void IMGV::searchThumbnails() const noexcept
{
    m_thumbnail_view->searchMode(true);
    m_thumbnail_search_edit->setVisible(true);
    m_thumbnail_search_edit->setFocus();
    m_thumbnail_search_edit->selectAll();
}

void IMGV::filterThumbnails() noexcept
{
    if (m_tags.isEmpty())
    {
        QMessageBox::information(this, "Filter by Tag", "No tags found in this session.");
        return;
    }

    QInputDialog dialog;
    dialog.setOptions(QInputDialog::UseListViewForComboBoxItems);
    dialog.setComboBoxItems(m_tags);
    dialog.setWindowTitle("Filter by Tag");

    connect(&dialog, &QInputDialog::textValueSelected, [&](const QString &tag) {
        m_thumbnail_view->filterMode(true);
        m_thumbnail_view->filter(tag);
    });

    dialog.exec();

}

void IMGV::createTag() noexcept
{
    QString new_tag_name = QInputDialog::getText(this, "Create Tag", "Please enter a tag name");
    
    if (new_tag_name.isEmpty())
    {
        QMessageBox::critical(this, "Error Creating Tag", "Tag name cannot be empty");
        return;
    }

    // If the new tag name is already existing in the list of tags, do nothing
    if (m_tags.indexOf(new_tag_name) > -1)
    {
        QMessageBox::critical(this, "Error Creating Tag", QString("Tag with the name `%1` already exists in this session. Please try again with a different name").arg(new_tag_name));
        return;
    }

    m_tags.push_back(new_tag_name);
}

void IMGV::assignTagToImage() noexcept
{
    if (m_tags.isEmpty())
    {
        QMessageBox::information(this, "Assign Tag", "No tags found in this session. Please create one first");
        return;
    }

    QInputDialog dialog;
    dialog.setOptions(QInputDialog::UseListViewForComboBoxItems);
    dialog.setComboBoxItems(m_tags);
    dialog.setWindowTitle("Assign Tag");

    connect(&dialog, &QInputDialog::textValueSelected, [&](const QString &tag) {
        m_thumbnail_view->model()->setTag(m_thumbnail_view->currentIndex(), tag);
    });

    dialog.exec();
}

void IMGV::manageTags(const bool &state) noexcept
{
    if (state)
    {
        if (m_tags.isEmpty())
        {

            QMessageBox::information(this, "Manage Tag", "No tags found in this session");
            tags_manage->setChecked(false);
            return;
        }

        m_manage_tag_dialog = new ManageTagDialog(m_tags, m_thumbnail_view->getAllThumbnails(), this);
        m_manage_tag_dialog->open();
        connect(m_manage_tag_dialog, &ManageTagDialog::visibilityChanged, tags_manage, [&](bool state) {
            tags_manage->setChecked(state);
        });
    }
    else
    {
        disconnect(m_manage_tag_dialog, 0, 0, 0);
        delete m_manage_tag_dialog;
        m_manage_tag_dialog = nullptr;
    }
}

void IMGV::processStdin() noexcept
{
    QFile file;
    QTemporaryFile tempPixFile;

    file.open(fileno(stdin), QIODevice::ReadOnly);
    tempPixFile.setAutoRemove(false);
    QByteArray data = file.readAll();
    file.close();
    if (tempPixFile.open())
    {
        tempPixFile.write(data);
        QString filename = tempPixFile.fileName();
        m_temp_files.push_back(filename);
        m_thumbnail_view->createThumbnail(filename);
        loadFile(filename);
    }
    else
        qDebug() << "Unable to read the image from stdin";
}

void IMGV::addSessionsToOpenSessionMenu() noexcept
{
    auto session_files = getSessionFiles();
    session__openSession->clear();

    for(const auto &file: session_files)
    {
        QAction *action = new QAction(QFileInfo(file).baseName());
        connect(action, &QAction::triggered, [&, action]() {
            QString filename = action->text() + ".imgv";
            QString f = QString("%1%2%3").arg(m_sessions_dir_path).arg(QDir::separator()).arg(filename);
            openSession(f);
            /*openSessionInNewWindow(file);*/
        });

        session__openSession->addAction(action);
    }
}

void IMGV::closeEvent(QCloseEvent *e) noexcept
{
    if (!m_temp_files.isEmpty())
    {
        for(const auto &file : m_temp_files)
        {
            QFile f(file);
            f.close();
            f.remove();
        }
    }
}

void IMGV::loadFile(const QString& fileName) noexcept
{
    QString title = QString(" %1 (Session: %2) | IMGV ").arg(fileName).arg(m_session_name);
    setWindowTitle(title);
    m_img_widget->loadFile(fileName);
}

void IMGV::loadFile(QString&& fileName) noexcept
{
    QString title = QString(" %1 (Session: %2) | IMGV ").arg(fileName).arg(m_session_name);
    setWindowTitle(title);
    m_img_widget->loadFile(fileName);
}
