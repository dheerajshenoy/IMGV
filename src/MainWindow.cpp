#include "MainWindow.hpp"

#include "ImageView.hpp"
#include "toml.hpp"

#include <QActionGroup>
#include <QClipboard>
#include <QDesktopServices>
#include <QFileDialog>
#include <QKeySequence>
#include <QMenuBar>
#include <QMessageBox>
#include <QProgressDialog>
#include <QScreen>
#include <QShortcut>
#include <QTabBar>
#include <QTimer>
#include <QWindow>

namespace
{

static inline void
set_title_format_if_present(toml::node_view<toml::node> n, QString &title_format)
{
    if (auto v = n.value<std::string>())
    {
        QString window_title = QString::fromStdString(*v);
        window_title.replace("{}", "%1");
        title_format = window_title;
    }
}

template <typename T>
static inline void
set_if_present(toml::node_view<toml::node> node, T &target)
{
    if (auto v = node.value<T>())
        target = *v;
}

static inline void
set_qstring_if_present(toml::node_view<toml::node> n, QString &dst)
{
    if (auto v = n.value<std::string>())
        dst = QString::fromStdString(*v);
}

static bool
parseHexColor(std::string_view s, uint32_t &out)
{
    if (s.empty())
        return false;
    if (s[0] == '#')
        s.remove_prefix(1);
    if (s.size() != 6 && s.size() != 8)
        return false;

    auto hex = [](char c) -> int
    {
        if (c >= '0' && c <= '9')
            return c - '0';
        if (c >= 'a' && c <= 'f')
            return 10 + (c - 'a');
        if (c >= 'A' && c <= 'F')
            return 10 + (c - 'A');
        return -1;
    };

    auto byte = [&](size_t i) -> int
    {
        int hi = hex(s[i]), lo = hex(s[i + 1]);
        if (hi < 0 || lo < 0)
            return -1;
        return (hi << 4) | lo;
    };

    int r = byte(0), g = byte(2), b = byte(4);
    if (r < 0 || g < 0 || b < 0)
        return false;
    int a = 255;
    if (s.size() == 8)
    {
        a = byte(6);
        if (a < 0)
            return false;
    }

    out = (uint32_t(r) << 24) | (uint32_t(g) << 16) | (uint32_t(b) << 8) | uint32_t(a);
    return true;
}

static inline void
set_color_if_present(toml::node_view<toml::node> n, uint32_t &dst)
{
    if (auto s = n.value<std::string>())
    {
        uint32_t tmp = dst;
        if (parseHexColor(*s, tmp))
            dst = tmp;
    }
}
} // namespace

void
MainWindow::readArgs(argparse::ArgumentParser &parser) noexcept
{

    // Check if version or commands flag is used
    if (parser.is_used("version"))
    {
        qDebug() << "Iv VERSION " << APP_VERSION;
        exit(0);
    }

    if (parser.is_used("commands"))
    {
        initCommandMap();
        qInfo() << "Available commands:\n";
        for (auto it = m_commandMap.begin(); it != m_commandMap.end(); it++)
            qInfo() << it.key();
        exit(0);
    }

    if (parser.is_used("config"))
    {
        const QString configPath = QString::fromStdString(parser.get<std::string>("config"));
        if (!configPath.isEmpty())
            m_config_file_path = configPath;
    }
    else
    {
        m_config_file_path = CONFIG_DIR + "config.toml";
    }

    if (parser.is_used("not-tabbed"))
        m_not_tabbed = true;

    if (parser.is_used("files"))
    {
        auto files = parser.get<std::vector<std::string>>("files");
        if (m_not_tabbed)
            OpenFiles(files);
        else
        {
            // Construct the main window
            this->construct();
            OpenFiles(files);
        }
    }
    else
    {
        // Construct the main window
        this->construct();
    }
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setAttribute(Qt::WA_NativeWindow);
    Magick::InitializeMagick(nullptr);
    setMinimumSize(600, 400);
}

void
MainWindow::construct() noexcept
{
    initCommandMap();
    initConfig();
    initConnections();
    initGui();
}

void
MainWindow::initGui() noexcept
{

    QVBoxLayout *layout = new QVBoxLayout();

    if (m_config.statusbar.location == "top")
    {
        layout->addWidget(m_panel);
        layout->addWidget(m_tab_widget);
    }
    else
    {
        layout->addWidget(m_tab_widget);
        layout->addWidget(m_panel);
    }

    QWidget *widget = new QWidget();
    widget->setLayout(layout);
    setCentralWidget(widget);

    m_file_menu = menuBar()->addMenu("&File");
    m_edit_menu = menuBar()->addMenu("&Edit");
    m_view_menu = menuBar()->addMenu("&View");
    m_help_menu = menuBar()->addMenu("&Help");

    m_open_file_action = m_file_menu->addAction(QString("Open File\t%1").arg(m_config.shortcutMap["open_file"]), this,
                                                &MainWindow::openFileDialog);

    m_recent_files_menu = m_file_menu->addMenu("Recent Files");
    m_recent_file_manager->PopulateRecentFilesMenu(
        m_recent_files_menu,
        std::function<void(const QString &)>([this](const QString &filepath) { OpenFile(filepath); }));

    m_open_containing_folder_action = m_file_menu->addAction(
        QString("Open Containing Folder\t%1").arg(m_config.shortcutMap["open_containing_folder"]), this,
        &MainWindow::OpenContainingFolder);

    m_copy_menu = m_edit_menu->addMenu("Copy");

    m_copy_path_action  = m_copy_menu->addAction(QString("File Path\t%1").arg(m_config.shortcutMap["copy_path"]), this,
                                                 &MainWindow::CopyFilePathToClipboard);
    m_copy_dir_action   = m_copy_menu->addAction(QString("File Directory\t%1").arg(m_config.shortcutMap["copy_dir"]),
                                                 this, &MainWindow::CopyFileDirToClipboard);
    m_copy_image_action = m_copy_menu->addAction(QString("Image\t%1").arg(m_config.shortcutMap["copy_image"]), this,
                                                 &MainWindow::CopyImageToClipboard);

    m_copy_viewport_action = m_copy_menu->addAction(QString("Viewport\t%1").arg(m_config.shortcutMap["copy_viewport"]),
                                                    this, &MainWindow::CopyImageToClipboard);

    m_file_properties_action =
        m_file_menu->addAction(QString("File Properties\t%1").arg(m_config.shortcutMap["file_properties"]), this,
                               &MainWindow::ShowFileProperties);

    m_close_file_action = m_file_menu->addAction(QString("Close File\t%1").arg(m_config.shortcutMap["close_file"]),
                                                 this, &MainWindow::CloseFile);
    m_exit_action =
        m_file_menu->addAction(QString("Exit\t%1").arg(m_config.shortcutMap["exit"]), this, &MainWindow::close);

    m_reset_view_action = m_view_menu->addAction(QString("Reset View\t%1").arg(m_config.shortcutMap["reset_view"]),
                                                 this, &MainWindow::ResetView);

    m_view_menu->addSeparator();

    m_zoom_menu = m_view_menu->addMenu("Zoom");

    m_zoom_in_action =
        m_zoom_menu->addAction(QString("In\t%1").arg(m_config.shortcutMap["zoom_in"]), this, &MainWindow::ZoomIn);

    m_zoom_out_action =
        m_zoom_menu->addAction(QString("Out\t%1").arg(m_config.shortcutMap["zoom_out"]), this, &MainWindow::ZoomOut);

    m_zoom_reset_action = m_zoom_menu->addAction(QString("Reset\t%1").arg(m_config.shortcutMap["zoom_reset"]), this,
                                                 &MainWindow::ZoomReset);

    m_rotate_menu         = m_view_menu->addMenu("Rotate");
    m_rotate_clock_action = m_rotate_menu->addAction(QString("Clockwise\t%1").arg(m_config.shortcutMap["rotate_clock"]),
                                                     this, &MainWindow::RotateClock);
    m_rotate_anticlock_action = m_rotate_menu->addAction(
        QString("Anticlockwise\t%1").arg(m_config.shortcutMap["rotate_anticlock"]), this, &MainWindow::RotateAnticlock);

    m_fit_menu = m_view_menu->addMenu("Fit");

    m_fit_width_action =
        m_fit_menu->addAction(QString("Width\t%1").arg(m_config.shortcutMap["fit_width"]), this, &MainWindow::FitWidth);
    m_fit_height_action = m_fit_menu->addAction(QString("Height\t%1").arg(m_config.shortcutMap["fit_height"]), this,
                                                &MainWindow::FitHeight);
    m_fit_window_action = m_fit_menu->addAction(QString("Window\t%1").arg(m_config.shortcutMap["fit_window"]), this,
                                                &MainWindow::FitWindow);

    m_auto_fit_action = m_fit_menu->addAction(QString("Auto Fit\t%1").arg(m_config.shortcutMap["auto_fit"]), this,
                                              &MainWindow::ToggleAutoFit);
    m_auto_fit_action->setCheckable(true);
    m_auto_fit_action->setChecked(m_config.behavior.auto_fit);

    m_flip_menu = m_view_menu->addMenu("Flip");

    m_flip_horizontal_action = m_flip_menu->addAction(
        QString("Horizontal\t%1").arg(m_config.shortcutMap["flip_horizontal"]), [&]() { Flip(Direction::LEFT); });
    m_flip_vertical_action = m_flip_menu->addAction(QString("Vertical\t%1").arg(m_config.shortcutMap["flip_vertical"]),
                                                    [&]() { Flip(Direction::UP); });

    m_toggle_menu           = m_view_menu->addMenu("Toggle");
    m_toggle_minimap_action = m_toggle_menu->addAction(
        QString("Minimap\t%1").arg(m_config.shortcutMap["toggle_minimap"]), this, &MainWindow::ToggleMinimap);
    m_toggle_minimap_action->setCheckable(true);
    m_toggle_minimap_action->setChecked(m_config.minimap.shown);

    m_toggle_menubar_action = m_toggle_menu->addAction(
        QString("Menubar\t%1").arg(m_config.shortcutMap["toggle_menubar"]), this, &MainWindow::ToggleMenubar);
    m_toggle_menubar_action->setCheckable(true);
    m_toggle_menubar_action->setChecked(m_config.menubar.shown);

    m_toggle_tabbar_action = m_toggle_menu->addAction(QString("Tabs\t%1").arg(m_config.shortcutMap["toggle_tabs"]),
                                                      this, &MainWindow::ToggleTabBar);
    m_toggle_tabbar_action->setCheckable(true);
    m_toggle_tabbar_action->setChecked(m_config.tabs.shown);

    m_toggle_hscrollbar_action = m_toggle_menu->addAction(
        QString("H Scrollbar\t%1").arg(m_config.shortcutMap["toggle_hscrollbar"]), this, &MainWindow::ToggleHScrollBar);
    m_toggle_hscrollbar_action->setCheckable(true);
    m_toggle_hscrollbar_action->setChecked(m_config.tabs.shown);

    m_toggle_vscrollbar_action = m_toggle_menu->addAction(
        QString("V Scrollbar\t%1").arg(m_config.shortcutMap["toggle_vscrollbar"]), this, &MainWindow::ToggleVScrollBar);
    m_toggle_vscrollbar_action->setCheckable(true);
    m_toggle_vscrollbar_action->setChecked(m_config.tabs.shown);

    m_toggle_panel_action = m_toggle_menu->addAction(
        QString("Statusbar\t%1").arg(m_config.shortcutMap["toggle_statusbar"]), this, &MainWindow::ToggleStatusbar);
    m_toggle_panel_action->setCheckable(true);
    m_toggle_panel_action->setChecked(m_config.statusbar.shown);

    m_toggle_auto_reload_action = m_toggle_menu->addAction(
        QString("Auto Reload\t%1").arg(m_config.shortcutMap["auto_reload"]), this, &MainWindow::ToggleAutoReload);
    m_toggle_auto_reload_action->setCheckable(true);
    m_toggle_auto_reload_action->setChecked(m_config.behavior.auto_reload);

    m_help_menu->addAction("About", this, [&]()
    {
        // TODO: Add custom widget
        QMessageBox::information(
            this, "About iv",
            QString("iv version %1\n\nA simple and fast image viewer bt with Qt and ImageMagick.")
                .arg(APP_VERSION));
    });

    this->setContentsMargins(0, 0, 0, 0);
    m_tab_widget->setContentsMargins(0, 0, 0, 0);
    layout->setContentsMargins(0, 0, 0, 0);
    widget->setContentsMargins(0, 0, 0, 0);

    // Remove frame and padding around tab widget to make it flush with window edges
    m_tab_widget->tabBar()->setStyleSheet("QTabBar { margin: 0; padding: 0; }");
    layout->setSpacing(0);
    m_tab_widget->setStyleSheet("border: 0;");
    m_tab_widget->tabBar()->setVisible(m_config.tabs.shown);
    m_tab_widget->setTabBarAutoHide(m_config.tabs.autohide);

    menuBar()->setVisible(m_config.menubar.shown);
    m_panel->setVisible(m_config.statusbar.shown);

    updateMenuActions(false);
    this->show();
}

void
MainWindow::initDefaultKeybinds() noexcept
{
    if (!m_default_keybindings)
        return;

    m_config.shortcutMap["m"]            = "toggle_minimap";
    m_config.shortcutMap["Ctrl+Shift+M"] = "toggle_menubar";
    m_config.shortcutMap["Ctrl+W"]       = "close_file";
    m_config.shortcutMap["o"]            = "open_file";
    m_config.shortcutMap["q"]            = "open_containing_folder";
    m_config.shortcutMap["="]            = "zoom_in";
    m_config.shortcutMap["-"]            = "zoom_out";
    m_config.shortcutMap[">"]            = "rotate_clock";
    m_config.shortcutMap["<"]            = "rotate_anticlock";
    m_config.shortcutMap["1"]            = "fit_width";
    m_config.shortcutMap["2"]            = "fit_height";
    m_config.shortcutMap["h"]            = "scroll_left";
    m_config.shortcutMap["j"]            = "scroll_down";
    m_config.shortcutMap["k"]            = "scroll_up";
    m_config.shortcutMap["l"]            = "scroll_right";
    m_config.shortcutMap["t"]            = "toggle_tabs";
    m_config.shortcutMap["F11"]          = "toggle_fullscreen";

    for (auto iter = m_config.shortcutMap.begin(); iter != m_config.shortcutMap.end(); iter++)
    {
        QShortcut *shortcut = new QShortcut(QKeySequence(iter.key()), this);
        connect(shortcut, &QShortcut::activated, this, [this, iter]() { m_commandMap[iter.value()](); });
    }
}

void
MainWindow::initConnections() noexcept
{
    QList<QScreen *> outputs = QGuiApplication::screens();
    connect(m_tab_widget, &QTabWidget::currentChanged, this, &MainWindow::handleCurrentTabChanged);

    QWindow *win = window()->windowHandle();

    m_dpr = m_screen_dpr_map.value(QGuiApplication::primaryScreen()->name(), 1.0f);

    connect(win, &QWindow::screenChanged, this, [&](QScreen *screen)
    {
        if (std::holds_alternative<QMap<QString, float>>(m_config.rendering.dpr))
        {
            m_dpr = m_screen_dpr_map.value(screen->name(), 1.0f);
            if (m_imgv)
                m_imgv->setDPR(m_dpr);
        }
        else if (std::holds_alternative<float>(m_config.rendering.dpr))
        {
            m_dpr = std::get<float>(m_config.rendering.dpr);
        }
    });

    connect(m_tab_widget, &TabWidget::currentChanged, [&](int index)
    {
        m_imgv = qobject_cast<ImageView *>(m_tab_widget->widget(index));
        updateFileinfoInPanel();
    });

    connect(m_tab_widget, &TabWidget::tabCloseRequested, this, &MainWindow::handleTabClose);
}

void
MainWindow::handleTabClose(int index) noexcept
{
    m_panel->clear();
    QWidget *widget = m_tab_widget->widget(index);
    if (!widget)
        return;
    widget->close();
    widget->deleteLater();
    m_tab_widget->removeTab(index);
}

void
MainWindow::OpenFiles(const QList<QString> &files) noexcept
{
    QProgressDialog progress("Opening files...", "Cancel", 0, files.size(), this);
    progress.setWindowModality(Qt::WindowModal);
    progress.setMinimumDuration(0);
    for (const QString &filepath : files)
    {
        OpenFile(filepath);
        progress.setValue(progress.value() + 1);
        if (progress.wasCanceled())
            break;
    }
    progress.close();
}

void
MainWindow::OpenFiles(const std::vector<std::string> &files) noexcept
{
    QProgressDialog progress("Opening files...", "Cancel", 0, files.size(), this);
    progress.setWindowModality(Qt::WindowModal);
    progress.setMinimumDuration(0);
    for (const std::string &filepath : files)
    {
        OpenFile(QString::fromStdString(filepath));
        progress.setValue(progress.value() + 1);
        if (progress.wasCanceled())
            break;
    }
    progress.close();
}

void
MainWindow::OpenFileInNewWindow(const QString &filepath) noexcept
{
    MainWindow *win = new MainWindow();
    win->construct();
    // win->show();
    win->OpenFile(filepath);
}

void
MainWindow::OpenFile(const QString &filepath) noexcept
{
    if (m_not_tabbed)
    {
        OpenFileInNewWindow(filepath);
        return;
    }

    QString fp = filepath;

    if (fp.isEmpty())
    {
        QStringList filepaths = openFileDialog();
        OpenFiles(filepaths);
        return;
    }

    if (QFileInfo(fp).isRelative() && m_config.statusbar.filepath_complete)
    {
        fp = QDir::current().absoluteFilePath(fp);
    }

    if (fp.startsWith("~"))
        fp = fp.replace(0, 1, QString::fromLocal8Bit(getenv("HOME")));

    m_imgv = new ImageView(m_config, m_tab_widget);

    bool success = m_imgv->openFile(fp);
    if (!success)
    {
        qWarning() << "Failed to open file:" << fp;
        QMessageBox::warning(this, "Open File Error", QString("Failed to open file:\n%1").arg(fp));

        m_imgv->deleteLater();
        m_imgv = nullptr;
    }
    else
    {
        updateMenuActions(true);
        updateFileinfoInPanel();

        m_recent_file_manager->addFilePath(fp);

        if (m_config.behavior.auto_reload)
            m_imgv->setAutoReload(true);

        m_tab_widget->addTab(m_imgv, fp);
        m_tab_widget->setCurrentWidget(m_imgv); // Make it the active tab

        connect(m_imgv, &ImageView::openFilesRequested, this,
                [&](const QStringList &files) { OpenFiles(files); }); // drop event
    }
}

void
MainWindow::CloseFile() noexcept
{
    if (m_tab_widget->currentIndex() >= 0)
        m_tab_widget->tabCloseRequested(m_tab_widget->currentIndex()); // Make it the active tab
}

void
MainWindow::ZoomIn() noexcept
{
    m_imgv->zoomIn();
}

void
MainWindow::ZoomOut() noexcept
{
    if (m_imgv)
        m_imgv->zoomOut();
}

void
MainWindow::ZoomReset() noexcept
{
    if (m_imgv)
        m_imgv->zoomReset();
}

void
MainWindow::RotateClock() noexcept
{
    if (m_imgv)
        m_imgv->rotateClock();
}

void
MainWindow::RotateAnticlock() noexcept
{
    if (m_imgv)
        m_imgv->rotateAnticlock();
}

void
MainWindow::FitHeight() noexcept
{
    if (m_imgv)
        m_imgv->fitHeight();
}

void
MainWindow::FitWidth() noexcept
{
    if (m_imgv)
        m_imgv->fitWidth();
}

void
MainWindow::FitWindow() noexcept
{
    if (m_imgv)
        m_imgv->fitWindow();
}

void
MainWindow::ToggleAutoFit() noexcept
{
    // Fit when window is resized
    if (m_imgv)
        m_imgv->setAutoFit(!m_imgv->autoFit());
}

void
MainWindow::ToggleAutoReload() noexcept
{
    if (m_imgv)
        m_imgv->toggleAutoReload();
}

void
MainWindow::ToggleMenubar() noexcept
{
    bool isVisible = menuBar()->isVisible();
    menuBar()->setVisible(!isVisible);
}

void
MainWindow::Scroll(Direction dir) noexcept
{
    switch (dir)
    {
        case Direction::LEFT:
            m_imgv->scrollLeft();
            break;

        case Direction::RIGHT:
            m_imgv->scrollRight();
            break;

        case Direction::UP:
            m_imgv->scrollUp();
            break;

        case Direction::DOWN:
            m_imgv->scrollDown();
            break;
    }
}

void
MainWindow::handleFileDrop() noexcept
{
}

void
MainWindow::ToggleMinimap() noexcept
{
    m_imgv->toggleMinimap();
}

void
MainWindow::dropEvent(QDropEvent *e)
{
    if (e->mimeData()->hasUrls())
    {
        QList<QString> files;
        for (const QUrl &url : e->mimeData()->urls())
        {
            QString file = url.toLocalFile();
            files.append(file);
        }
        OpenFiles(files);
    }
}

void
MainWindow::dragEnterEvent(QDragEnterEvent *e)
{
    if (e->mimeData()->hasUrls() || e->mimeData()->hasText())
        e->acceptProposedAction();
    else
        e->ignore();
}

QStringList
MainWindow::openFileDialog() noexcept
{
    QStringList extensions = {
#ifdef HAS_LIBAVIF
        "*.avif",
#endif
        "*.jpg",  "*.bmp",  "*.cgm", "*.dpx", "*.emf", "*.exr",  "*.fits", "*.gif", "*.heic", "*.heif",
        "*.jp2",  "*.jpeg", "*.jxl", "*.pcx", "*.png", "*.psd",  "*.sgi",  "*.svg", "*.tga",  "*.tiff",
        "*.ico",  "*.webp", "*.wmf", "*.xbm", "*.cr2", "*.crw",  "*.dds",  "*.eps", "*.raf",  "*.jng",
        "*.dcr",  "*.mrw",  "*.nef", "*.orf", "*.pef", "*.pict", "*.pnm",  "*.pbm", "*.pgm",  "*.ppm",
        "*.rgb",  "*.arw",  "*.srf", "*.sr2", "*.xcf", "*.xpm"};

    QString filter = QString("Image Files (%1);;All Files (*)").arg(extensions.join(' '));

    return QFileDialog::getOpenFileNames(this, "Open File", QString(), filter);
}

void
MainWindow::updateFileinfoInPanel() noexcept
{
    if (!m_imgv)
        return;

    QString filepath = m_imgv->filePath();

    if (m_config.statusbar.filepath_complete && QFileInfo(filepath).isRelative())
    {
        filepath = QDir::current().absoluteFilePath(filepath);
    }
    else
    {
        filepath = QFileInfo(filepath).fileName();
    }

    const QSize &size = m_imgv->size();
    m_panel->setFileName(filepath);
    m_panel->setImageSize(size.width(), size.height());
    m_panel->setFileSize(m_imgv->fileSize());
    m_imgv->updateMinimapPosition();
    this->setWindowTitle(QString("iv: %1").arg(filepath));
}

void
MainWindow::initConfig() noexcept
{
    toml::table toml;

    try
    {
        toml = toml::parse_file(m_config_file_path.toStdString());
    }
    catch (const toml::parse_error &e)
    {
        QMessageBox::critical(this, "Config Error", e.what());
        return;
    }

    auto tabs = toml["tabs"];

    if (tabs)
    {
        set_if_present(tabs["shown"], m_config.tabs.shown);
        set_if_present(tabs["auto_hide"], m_config.tabs.autohide);
        set_qstring_if_present(tabs["location"], m_config.tabs.location);
    }

    auto menubar = toml["menubar"];

    if (menubar)
    {
        set_if_present(menubar["shown"], m_config.menubar.shown);
    }

    auto statusbar = toml["statusbar"];

    if (statusbar)
    {
        set_if_present(statusbar["shown"], m_config.statusbar.shown);
        set_qstring_if_present(statusbar["position"], m_config.statusbar.location);
        set_if_present(statusbar["filepath_complete"], m_config.statusbar.filepath_complete);
    }

    auto hscrollbar = toml["hscrollbar"];

    if (hscrollbar)
    {
        set_if_present(hscrollbar["shown"], m_config.hscrollbar.shown);
        set_if_present(hscrollbar["auto_hide"], m_config.hscrollbar.auto_hide);
    }

    auto vscrollbar = toml["vscrollbar"];

    if (vscrollbar)
    {
        set_if_present(vscrollbar["shown"], m_config.vscrollbar.shown);
        set_if_present(vscrollbar["auto_hide"], m_config.vscrollbar.auto_hide);
    }

    auto minimap = toml["minimap"];

    if (minimap)
    {
        set_if_present(minimap["shown"], m_config.minimap.shown);
        set_if_present(minimap["auto_hide"], m_config.minimap.auto_hide);
        set_if_present(minimap["padding"], m_config.minimap.padding);
        set_if_present(minimap["clickable"], m_config.minimap.clickable);
        set_color_if_present(minimap["border_color"], m_config.minimap.border_color);
        set_if_present(minimap["border_width"], m_config.minimap.border_width);

        if (minimap["size"].is_table())
        {
            const auto size_table = minimap["size"];
            const int width       = size_table["width"].value_or(200);
            const int height      = size_table["height"].value_or(200);
            m_config.minimap.size = QSize(width, height);
        }
    }

    auto overlay = toml["minimap"]["overlay"];

    if (overlay)
    {
        set_if_present(overlay["movable"], m_config.minimap.overlay.movable);
        set_color_if_present(overlay["color"], m_config.minimap.overlay.color);
        set_color_if_present(overlay["border"], m_config.minimap.overlay.border_color);
        set_if_present(overlay["border_width"], m_config.minimap.overlay.border_width);
    }

    m_config.minimap.image         = minimap["image"].value_or(true);
    m_config.minimap.image_opacity = minimap["image_opacity"].value_or(0.7f);
    const QString minimap_location = minimap["location"].value_or("bottom-right");

    Minimap::Location loc;
    if (minimap_location == "top-left")
        loc = Minimap::Location::TOP_LEFT;
    else if (minimap_location == "top-center")
        loc = Minimap::Location::TOP_CENTER;
    else if (minimap_location == "top-right")
        loc = Minimap::Location::TOP_RIGHT;
    else if (minimap_location == "bottom-left")
        loc = Minimap::Location::BOTTOM_LEFT;
    else if (minimap_location == "bottom-center")
        loc = Minimap::Location::BOTTOM_CENTER;
    else if (minimap_location == "center")
        loc = Minimap::Location::CENTER;
    else if (minimap_location == "center-left")
        loc = Minimap::Location::CENTER_LEFT;
    else if (minimap_location == "center-right")
        loc = Minimap::Location::CENTER_RIGHT;
    else
        loc = Minimap::Location::BOTTOM_RIGHT;
    m_config.minimap.location = loc;

    auto focus_mode = toml["focus_mode"];

    if (focus_mode)
    {
        m_config.focus_mode.statusbar_shown  = focus_mode["statusbar"].value_or(false);
        m_config.focus_mode.menubar_shown    = focus_mode["menubar"].value_or(false);
        m_config.focus_mode.minimap_shown    = focus_mode["minimap"].value_or(false);
        m_config.focus_mode.hscrollbar_shown = focus_mode["hscrollbar"].value_or(false);
        m_config.focus_mode.vscrollbar_shown = focus_mode["vscrollbar"].value_or(false);
        m_config.focus_mode.tabs_shown       = focus_mode["tabs"].value_or(false);
    }

    auto behavior = toml["behavior"];

    if (behavior)
    {
        m_config.behavior.auto_reload              = behavior["auto_reload"].value_or(false);
        m_config.behavior.save_recent_files        = behavior["save_recent_files"].value_or(true);
        m_config.behavior.recent_files_limit       = behavior["recent_files_limit"].value_or(10);
        m_config.behavior.auto_fit                 = behavior["auto_fit"].value_or(false);
        m_config.behavior.keybind_conflict_warning = behavior["keybind_conflict_warning"].value_or(true);
        m_config.behavior.copy_transformed_image   = behavior["copy_transformed_image"].value_or(false);
    }

    auto rendering = toml["rendering"];

    // If DPR is specified in config, use that (can be scalar or map)
    if (rendering)
    {
        if (rendering["dpr"]) {
            if (rendering["dpr"].is_value())
            {
                m_config.rendering.dpr = rendering["dpr"].value_or(1.0f); // scalar
            }
            else if (rendering["dpr"].is_table())
            {
                auto dpr_table = rendering["dpr"];
                for (auto &[screen_name, value] : *dpr_table.as_table())
                {
                    float dpr_value          = value.value_or(1.0f);
                    QString screen_str       = QString::fromStdString(std::string(screen_name.str()));
                    QList<QScreen *> screens = QApplication::screens();
                    for (QScreen *screen : screens)
                    {
                        if (screen->name() == screen_str)
                        {
                            m_screen_dpr_map[screen->name()] = dpr_value;
                            break;
                        }
                    }
                }

                m_config.rendering.dpr = m_screen_dpr_map;
            }
        }
    }
    else
    {
        m_screen_dpr_map[QApplication::primaryScreen()->name()] =
            QApplication::primaryScreen()->devicePixelRatio();
        m_config.rendering.dpr = m_screen_dpr_map.value(QApplication::primaryScreen()->name(), 1.0f);
    }

    // Read Keybindings

    auto keys = toml["keybindings"];

    if (keys)
    {
        for (auto &[action, value] : *keys.as_table())
        {
            if (value.is_value())
                setupKeybinding(QString::fromStdString(std::string(action.str())),
                        QString::fromStdString(value.value_or<std::string>("")));
        }
    }

    if (m_config.behavior.save_recent_files)
    {
        if (!m_recent_file_manager)
            m_recent_file_manager =
                new RecentFilesManager(CONFIG_DIR + "recent_files.json", m_config.behavior.recent_files_limit);
    }
}

void
MainWindow::initCommandMap() noexcept
{
    m_commandMap["reset_view"] = [this]()
    {
        ResetView();
    };

    m_commandMap["copy_path"] = [this]()
    {
        CopyFilePathToClipboard();
    };

    m_commandMap["copy_dir"] = [this]()
    {
        CopyFileDirToClipboard();
    };

    m_commandMap["copy_image"] = [this]()
    {
        CopyImageToClipboard();
    };

    m_commandMap["copy_viewport"] = [this]()
    {
        CopyViewportImageToClipboard();
    };

    m_commandMap["reload_file"] = [this]()
    {
        if (m_imgv)
        {
            QString filepath = m_imgv->filePath();
            m_imgv->reloadFile();
        }
    };

    m_commandMap["left_edge"] = [this]()
    {
        m_imgv->scrollToLeftEdge();
    };

    m_commandMap["right_edge"] = [this]()
    {
        m_imgv->scrollToRightEdge();
    };

    m_commandMap["top_edge"] = [this]()
    {
        m_imgv->scrollToTopEdge();
    };

    m_commandMap["bottom_edge"] = [this]()
    {
        m_imgv->scrollToBottomEdge();
    };

    m_commandMap["open_file"] = [this]()
    {
        OpenFile();
    };

    m_commandMap["close_file"] = [this]()
    {
        CloseFile();
    };

    m_commandMap["toggle_tabs"] = [this]()
    {
        ToggleTabBar();
    };

    m_commandMap["toggle_statusbar"] = [this]()
    {
        ToggleStatusbar();
    };

    m_commandMap["toggle_focus_mode"] = [this]()
    {
        ToggleFocusMode();
    };

    m_commandMap["toggle_fullscreen"] = [this]()
    {
        if (this->isFullScreen())
            this->showNormal();
        else
            this->showFullScreen();
    };

    m_commandMap["toggle_menubar"] = [this]()
    {
        ToggleMenubar();
    };

    m_commandMap["zoom_in"] = [this]()
    {
        ZoomIn();
    };
    m_commandMap["zoom_out"] = [this]()
    {
        ZoomOut();
    };

    m_commandMap["zoom_reset"] = [this]()
    {
        ZoomReset();
    };

    m_commandMap["rotate_clock"] = [this]()
    {
        RotateClock();
    };

    m_commandMap["rotate_anticlock"] = [this]()
    {
        RotateAnticlock();
    };

    m_commandMap["fit_width"] = [this]()
    {
        FitWidth();
    };

    m_commandMap["fit_height"] = [this]()
    {
        FitHeight();
    };

    m_commandMap["fit_window"] = [this]()
    {
        FitWindow();
    };

    m_commandMap["auto_fit"] = [this]()
    {
        ToggleAutoFit();
    };

    m_commandMap["file_properties"] = [this]()
    {
        ShowFileProperties();
    };

    m_commandMap["scroll_left"] = [this]()
    {
        Scroll(Direction::LEFT);
    };

    m_commandMap["scroll_down"] = [this]()
    {
        Scroll(Direction::DOWN);
    };

    m_commandMap["scroll_up"] = [this]()
    {
        Scroll(Direction::UP);
    };

    m_commandMap["scroll_right"] = [this]()
    {
        Scroll(Direction::RIGHT);
    };

    m_commandMap["toggle_minimap"] = [this]()
    {
        ToggleMinimap();
    };

    m_commandMap["flip_horizontal"] = [this]()
    {
        Flip(Direction::LEFT);
    };

    m_commandMap["flip_vertical"] = [this]()
    {
        Flip(Direction::UP);
    };

    // Command to switch to tabs 0-9 and also next and prev tab
    m_commandMap["next_tab"] = [this]()
    {
        NextTab();
    };
    m_commandMap["prev_tab"] = [this]()
    {
        PrevTab();
    };

    m_commandMap["tab_last"] = [this]()
    {
        int lastIndex = m_tab_widget->count() - 1;
        SwitchToTab(lastIndex);
    };

    m_commandMap["open_containing_folder"] = [this]()
    {
        OpenContainingFolder();
    };

    for (int i = 1; i < 11; i++)
    {
        m_commandMap[QString("tab_%1").arg(i)] = [this, i]()
        {
            SwitchToTab(i - 1);
        };
    }
}

void
MainWindow::setupKeybinding(const QString &action, const QString &key) noexcept
{
    // Check for conflicts
    if (m_config.behavior.keybind_conflict_warning)
    {
        for (auto it = m_config.shortcutMap.begin(); it != m_config.shortcutMap.end(); ++it)
        {
            if (it.key() != action && it.value() == key)
            {
                QMessageBox::warning(this, "Keybinding Conflict",
                                     QString("The key '%1' is already assigned to action '%2'.\n"
                                             "Please resolve the conflict in the config file.")
                                         .arg(key, it.key()));
                return;
            }
        }
    }

    // Remove existing shortcut for this action, if any
    if (m_shortcut_map.contains(action))
    {
        QShortcut *existingShortcut = m_shortcut_map[action];
        existingShortcut->disconnect();
        existingShortcut->deleteLater();
        m_shortcut_map.remove(action);
    }

    const auto it = m_commandMap.find(action);
    if (it == m_commandMap.end())
        return;

    QShortcut *shortcut = new QShortcut(QKeySequence(key), this);
    connect(shortcut, &QShortcut::activated, this, [it]() { it.value()(); });
    m_config.shortcutMap[action] = key;
    m_shortcut_map[action]       = shortcut;
}

void
MainWindow::Flip(Direction dir) noexcept
{
    switch (dir)
    {
        case Direction::LEFT:
            m_imgv->flipLeftRight();
            break;

        case Direction::UP:
            m_imgv->flipUpDown();
            break;

        default:
            break;
    }
}

void
MainWindow::NextTab() noexcept
{
    int currentIndex = m_tab_widget->currentIndex();
    int tabCount     = m_tab_widget->count();
    if (tabCount == 0)
        return;
    int nextIndex = (currentIndex + 1) % tabCount;
    m_tab_widget->setCurrentIndex(nextIndex);
}

void
MainWindow::PrevTab() noexcept
{
    int currentIndex = m_tab_widget->currentIndex();
    int tabCount     = m_tab_widget->count();
    if (tabCount == 0)
        return;
    int prevIndex = (currentIndex - 1 + tabCount) % tabCount;
    m_tab_widget->setCurrentIndex(prevIndex);
}

void
MainWindow::SwitchToTab(int index) noexcept
{
    int tabCount = m_tab_widget->count();
    if (index < 0 || index >= tabCount)
        return;
    m_tab_widget->setCurrentIndex(index);
}

void
MainWindow::OpenContainingFolder() noexcept
{
    if (!m_imgv)
        return;
    QString filedir = m_imgv->fileDir();
    QDesktopServices::openUrl(QUrl(filedir));
}

void
MainWindow::updateMenuActions(bool state) noexcept
{
    m_zoom_menu->setEnabled(state);
    m_rotate_menu->setEnabled(state);
    m_fit_menu->setEnabled(state);
    m_close_file_action->setEnabled(state);
    m_toggle_auto_reload_action->setEnabled(state);
    m_open_containing_folder_action->setEnabled(state);
    m_file_properties_action->setEnabled(state);
    m_flip_menu->setEnabled(state);
}

void
MainWindow::handleCurrentTabChanged(int index) noexcept
{
    m_imgv = qobject_cast<ImageView *>(m_tab_widget->widget(index));
    updateMenuActions(m_imgv != nullptr);
    updateFileinfoInPanel();
}

void
MainWindow::resizeEvent(QResizeEvent *e)
{
    if (m_imgv && m_imgv->autoFit())
    {
        switch (m_imgv->fitMode())
        {
            case ImageView::FitMode::WIDTH:
                m_imgv->fitWidth();
                break;

            case ImageView::FitMode::HEIGHT:
                m_imgv->fitHeight();
                break;

            case ImageView::FitMode::WINDOW:
                m_imgv->fitWindow();
                break;

            default:
                break;
        }
    }

    QMainWindow::resizeEvent(e);
}

void
MainWindow::ShowFileProperties() noexcept
{
    if (!m_imgv)
        return;

    m_imgv->showFilePropertiesDialog();
}

QTabWidget::TabPosition
MainWindow::tabBarPositionFromString(const QString &locationStr) const noexcept
{
    QTabWidget::TabPosition loc;
    if (locationStr == "top")
        loc = QTabWidget::TabPosition::North;
    else if (locationStr == "bottom")
        loc = QTabWidget::TabPosition::South;
    else if (locationStr == "left")
        loc = QTabWidget::TabPosition::West;
    else if (locationStr == "right")
        loc = QTabWidget::TabPosition::East;
    else
        loc = QTabWidget::TabPosition::North;
    return loc;
}

void
MainWindow::ToggleTabBar() noexcept
{
    QTabBar *tabbar = m_tab_widget->tabBar();
    tabbar->setVisible(!tabbar->isVisible());
}

void
MainWindow::ToggleHScrollBar() noexcept
{
    if (m_imgv)
        m_imgv->toggleHScrollbar();
}

void
MainWindow::ToggleVScrollBar() noexcept
{
    if (m_imgv)
        m_imgv->toggleVScrollbar();
}

void
MainWindow::ToggleFocusMode() noexcept
{
    if (!m_imgv)
        return;

    m_focus_mode = !m_focus_mode;

    if (m_focus_mode)
    {
        // Hide UI elements for focus mode
        m_panel->setVisible(m_config.focus_mode.statusbar_shown);
        menuBar()->setVisible(m_config.focus_mode.menubar_shown);
        m_imgv->minimap()->setVisible(m_config.focus_mode.minimap_shown);
        m_tab_widget->tabBar()->setVisible(m_config.focus_mode.tabs_shown);
        m_imgv->setHScrollBarVisible(m_config.focus_mode.hscrollbar_shown);
        m_imgv->setVScrollBarVisible(m_config.focus_mode.vscrollbar_shown);
    }
    else
    {
        // Restore previous visibility states
        m_panel->setVisible(m_config.statusbar.shown);
        menuBar()->setVisible(m_config.menubar.shown);
        m_imgv->minimap()->setVisible(m_config.minimap.shown);
        updateTabBarVisibility();
        m_tab_widget->update();
        m_imgv->setHScrollBarVisible(m_config.hscrollbar.shown);
        m_imgv->setVScrollBarVisible(m_config.vscrollbar.shown);
    }
}

void
MainWindow::updateTabBarVisibility() noexcept
{
    // Let tab widget manage visibility itself based on auto-hide property
    m_tab_widget->tabBar()->setVisible(true); // initially show
    if (m_tab_widget->tabBarAutoHide() && m_tab_widget->count() < 2)
        m_tab_widget->tabBar()->setVisible(false);
}

void
MainWindow::ResetView() noexcept
{
    if (m_imgv)
        m_imgv->resetView();
}

void
MainWindow::CopyFilePathToClipboard() noexcept
{
    if (!m_imgv)
        return;

    const QString &filepath = m_imgv->filePath();
    QClipboard *clipboard   = QGuiApplication::clipboard();
    clipboard->setText(filepath);
}

void
MainWindow::CopyFileDirToClipboard() noexcept
{
    if (!m_imgv)
        return;

    const QString &filedir = m_imgv->fileDir();
    QClipboard *clipboard  = QGuiApplication::clipboard();
    clipboard->setText(filedir);
}

void
MainWindow::CopyImageToClipboard() noexcept
{
    if (!m_imgv)
        return;

    QImage img;
    if (m_config.behavior.copy_transformed_image)
    {
        img = m_imgv->transformedImage();
    }
    else
    {
        img = m_imgv->image();
    }
    QClipboard *clipboard = QGuiApplication::clipboard();
    clipboard->setImage(img);
}

void
MainWindow::CopyViewportImageToClipboard() noexcept
{
    if (!m_imgv)
        return;

    QImage img            = m_imgv->viewportImage();
    QClipboard *clipboard = QGuiApplication::clipboard();
    clipboard->setImage(img);
}
