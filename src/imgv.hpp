#ifndef IMGV_HPP
#define IMGV_HPP

#include "argparse.hpp"
#include <QApplication>
#include <QTemporaryFile>
#include <QMessageBox>
#include <QTextEdit>
#include <QLineEdit>
#include <QInputDialog>
#include <QMainWindow>
#include <QSplitter>
#include <QMenuBar>
#include <QAction>
#include <QPixmapCache>
#include <QShortcut>
#include <QKeySequence>
#include <QTimer>
#include <QDateTime>
#include <QStandardPaths>
#include <QProcess>
#include <QString>
#include <QFileDialog>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QVBoxLayout>
#include <QWidget>
#include "ImageWidget.hpp"
#include "StatusBar.hpp"
#include "sol/sol.hpp"
#include "AboutDialog.hpp"
#include "ThumbnailView.hpp"
#include "ThumbnailModel.hpp"
#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "rapidjson/reader.h"
#include "rapidjson/istreamwrapper.h"
#include <fstream>
#include "ManageSessionsDialog.hpp"
#include "NoteWidget.hpp"
#include "ThumbnailTools.hpp"
#include "ManageTagDialog.hpp"
#include <QByteArray>
#include <QCloseEvent>
#include <unistd.h>
#include <QActionGroup>
#include <QSizePolicy>

class IMGV : public QMainWindow
{

public:
    IMGV(argparse::ArgumentParser &parser, QWidget *parent = nullptr);
    ~IMGV() {}

protected:
    void closeEvent(QCloseEvent *e) noexcept override;

private:
    void initConfigDirectory() noexcept;
    void initDefaultConfig() noexcept;
    void initMenu() noexcept;
    void initConnections() noexcept;
    void initKeybinds() noexcept;
    void openImage() noexcept;
    void slideShow() noexcept;
    void toggleSlideshow() const noexcept;
    void saveSession() noexcept;
    void openImageInNewWindow() noexcept;
    void parseCommandLineArguments(const argparse::ArgumentParser &) noexcept;
    QStringList getSessionFiles() const noexcept;
    QStringList getSessionTags() noexcept;
    void readSessionFile(QString& filepath) noexcept;
    void readSessionFile(QString&& filepath) noexcept;
    void openSessionInNewWindow(const QString &) noexcept;
    void maximizeImage() noexcept;
    void closeSession() noexcept;
    void openSession(QString &) noexcept;
    void newSession() noexcept;
    void addNote() noexcept;
    void searchThumbnails() const noexcept;
    void filterThumbnails() noexcept;
    void toggleNotes() const noexcept;
    void createTag() noexcept;
    void assignTagToImage() noexcept;
    void manageTags(const bool &state) noexcept;
    void processStdin() noexcept;
    void addSessionsToOpenSessionMenu() noexcept;
    void addSessionToOpenSessionMenu(const QString &sessionName) noexcept;
    void cleanUp() noexcept;
    void loadFile(const QString& file) noexcept;
    void loadFile(QString&& file) noexcept;

    void thumbnailPanel__left() noexcept;
    void thumbnailPanel__right() noexcept;
    void thumbnailPanel__bottom() noexcept;
    void thumbnailPanel__top() noexcept;

    inline void setMsg(const QString &msg, const int sec = 1) const noexcept
    {
        m_statusbar->setMsg(msg, sec);
    }

    inline void toggleThumbnailPanel() const noexcept
    {
        m_thumbnail_view->setVisible(!m_thumbnail_view->isVisible());
    }

    inline void toggleStatusbar() const noexcept
    {
        m_statusbar->setVisible(!m_statusbar->isVisible());
    }

    inline void toggleMenubar() const noexcept
    {
        m_menuBar->setVisible(!m_menuBar->isVisible());
    }

    inline void ThumbSearchTextChanged(const QString text) const noexcept
    {
        m_thumbnail_view->search(text);
    }

    inline void fullScreen() noexcept
    {
        this->showFullScreen();
    }


    QWidget *m_centralWidget = new QWidget();
    QVBoxLayout *m_layout = new QVBoxLayout();
    QSplitter *m_splitter = new QSplitter();

    QStringList m_slideshow_files;
    QStringList m_tags;
    QStringList m_temp_files;

    ThumbnailView *m_thumbnail_view  = new ThumbnailView();
    ImageWidget *m_img_widget = new ImageWidget();
    StatusBar *m_statusbar = new StatusBar();

    bool m_slideshow_mode = false;
    QTimer *m_slideshow_timer = nullptr;

    QString m_config_dir_path, m_sessions_dir_path;
    sol::state m_lua_state;

    QMenuBar *m_menuBar = new QMenuBar();
    QMenu *fileMenu = m_menuBar->addMenu("&File");
    QMenu *sessionMenu = m_menuBar->addMenu("&Session");
    QMenu *editMenu = m_menuBar->addMenu("&Edit");
    QMenu *toolsMenu = m_menuBar->addMenu("&Tools");
    QMenu *viewMenu = m_menuBar->addMenu("&View");
    QMenu *helpMenu = m_menuBar->addMenu("&Help");

    QAction *file__openAction = new QAction("Open");
    QAction *file__openNewWindowAction = new QAction("Open in new &Window");
    QAction *file__exit = new QAction("Exit");

    QAction *session__newSession = new QAction("New");
    QMenu *session__openSession = new QMenu("Open");
    QAction *session__saveSession = new QAction("Save");
    QAction *session__closeSession = new QAction("Close");
    QMenu *session__tags = new QMenu("Tag");
    QAction *session__manage_sessions = new QAction("Manage Sessions");

    QAction *tags_assign = new QAction("Assign");
    QAction *tags_new = new QAction("New");
    QAction *tags_manage = new QAction("Manage");

    QAction *help__about = new QAction("About");

    QMenu *edit__zoom = new QMenu("Zoom");
    QMenu *edit__rotate = new QMenu("Rotate");
    QMenu *edit__flip = new QMenu("Flip");
    QMenu *edit__fit = new QMenu("Fit");
    QMenu *edit__fit_on_load = new QMenu("Fit on Load");

    QMenu *edit__sort = new QMenu("Sort");

    QAction *sort__name = new QAction("Name");
    QAction *sort__size = new QAction("Size");
    QAction *sort__date = new QAction("Date");

    QAction *zoom__in = new QAction("Zoom In");
    QAction *zoom__out = new QAction("Zoom Out");
    QAction *zoom__reset = new QAction("Zoom Reset");

    QAction *flip__vertical = new QAction("Vertical");
    QAction *flip__horizontal = new QAction("Horizontal");

    QAction *rotate__clockwise = new QAction("Clockwise");
    QAction *rotate__anticlockwise = new QAction("Anti Clockwise");
    QAction *rotate__custom = new QAction("Custom");
    QAction *rotate__reset = new QAction("Reset");

    QAction *fit__width = new QAction("Width");
    QAction *fit__height = new QAction("Height");

    QAction *fit_on_load__none = new QAction("None");
    QAction *fit_on_load__height = new QAction("Height");
    QAction *fit_on_load__width = new QAction("Width");

    QActionGroup *fit_on_load_action_group = new QActionGroup(edit__fit_on_load);

    QAction *view__minimap = new QAction("Minimap");
    QAction *view__thumbnails = new QAction("Thumbnail Panel");
    QAction *view__statusbar = new QAction("Statusbar");
    QAction *view__menubar = new QAction("Menubar");
    QAction *view__notes = new QAction("Notes");
    QAction *view__maximize_image = new QAction("Maximize Image");

    QAction *tools__pix_analyser = new QAction("Pixel Analyser");
    QAction *tools__slideshow = new QAction("Slideshow");

    QString m_session_name;
    QString m_session_date;
    bool m_image_maximize_mode = false;

    ThumbnailModel *m_thumbnail_model = new ThumbnailModel();
    QLineEdit *m_thumbnail_search_edit = new QLineEdit();

    QWidget *m_right_pane = new QWidget();
    QWidget *m_thumbnail_left_pane = nullptr;
    QWidget *m_thumbnail_right_pane = nullptr;
    QWidget *m_thumbnail_bottom_pane = nullptr;

    QVBoxLayout *m_thumbnail_left_pane_layout = nullptr;
    QVBoxLayout *m_thumbnail_right_pane_layout = nullptr;
    QHBoxLayout *m_thumbnail_bottom_pane_layout = nullptr;

    QVBoxLayout *m_thumbnail_tools_bottom_layout = nullptr;
    QVBoxLayout *m_thumbnail_tools_left_layout = nullptr;

    QVBoxLayout *m_right_pane_layout = nullptr;

    ThumbnailTools *m_thumbnail_tools_widget;
    NoteWidget *m_note_holder = new NoteWidget();
    QSplitter *m_right_pane_splitter = new QSplitter(Qt::Orientation::Horizontal);
    bool m_auto_notes_popup = false;

    int m_slideshow_index = -1;

    ManageTagDialog *m_manage_tag_dialog = nullptr;
    bool m_stdin = false;

    QString m_supported_image_formats;
    QStringList m_supported_image_formats_list;
};


#endif

