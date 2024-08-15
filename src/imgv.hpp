#ifndef IMGV_HPP
#define IMGV_HPP

#include "argparse.hpp"
#include <QApplication>
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
#include "exif.hpp"
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
#include "PixAnalyser.hpp"
#include "Minimap.hpp"
#include "ThumbnailTools.hpp"
#include "ManageTagDialog.hpp"
#include <QByteArray>
#include <unistd.h>

class IMGV : public QMainWindow
{

public:
    IMGV(argparse::ArgumentParser &parser, QWidget *parent = nullptr);
    ~IMGV(){}

private:
    void initConfigDirectory();
    void initDefaultConfig();
    void initMenu();
    void initConnections();
    void initKeybinds();
    void openImage();
    void slideShow() noexcept;
    void toggleSlideshow() noexcept;
    void saveSession();
    void openImageInNewWindow();
    void parseCommandLineArguments(argparse::ArgumentParser &);
    QStringList getSessionFiles();
    QStringList getSessionTags();
    void readSessionFile(QString filepath);
    void openSessionInNewWindow(QString &);
    void fullScreen();
    void maximizeImage();
    void closeSession();
    void openSession(QString &);
    void newSession() noexcept;
    void addNote() noexcept;
    void ThumbSearchTextChanged(QString) noexcept;
    void searchThumbnails() noexcept;
    void filterThumbnails() noexcept;
    void toggleNotes() noexcept;
    void toggleThumbnailPanel() noexcept;
    void toggleMenubar() noexcept;
    void toggleStatusbar() noexcept;
    void createTag() noexcept;
    void assignTagToImage() noexcept;
    void manageTags(const bool state) noexcept;
    void processStdin(QIODevice *inputDevice) noexcept;


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
    QMenu *file__openRecent = new QMenu("Open Recent Files");
    QAction *file__openNewWindowAction = new QAction("Open in new &Window");
    QAction *file__exit = new QAction("Exit");

    QAction *session__newSession = new QAction("New");
    QMenu *session__openSession = new QMenu("Open");
    QAction *session__saveSession = new QAction("Save");
    QAction *session__closeSession = new QAction("Close");
    QMenu *session__tags = new QMenu("Tag");

    QAction *tags_assign = new QAction("Assign");
    QAction *tags_new = new QAction("New");
    QAction *tags_manage = new QAction("Manage");

    QAction *help__about = new QAction("About");

    QMenu *edit__zoom = new QMenu("Zoom");
    QMenu *edit__rotate = new QMenu("Rotate");
    QMenu *edit__flip = new QMenu("Flip");
    QAction *edit__fit_on_load = new QAction("Fit on Load");

    QAction *zoom__in = new QAction("Zoom In");
    QAction *zoom__out = new QAction("Zoom Out");
    QAction *zoom__reset = new QAction("Zoom Reset");

    QAction *flip__vertical = new QAction("Vertical");
    QAction *flip__horizontal = new QAction("Horizontal");

    QAction *rotate__clockwise = new QAction("Clockwise");
    QAction *rotate__anticlockwise = new QAction("Anti Clockwise");
    QAction *rotate__custom = new QAction("Custom");
    QAction *rotate__reset = new QAction("Reset");

    QAction *view__minimap = new QAction("Minimap");
    QAction *view__thumbnails = new QAction("Thumbnail Panel");
    QAction *view__statusbar = new QAction("Statusbar");
    QAction *view__menubar = new QAction("Menubar");
    QAction *view__notes = new QAction("Notes");
    QAction *view__maximize_image = new QAction("Maximize Image");

    QAction *tools__manage_sessions = new QAction("Manage Sessions");
    QAction *tools__pix_analyser = new QAction("Pixel Analyser");
    QAction *tools__slideshow = new QAction("Slideshow");

    QString m_session_name;
    QString m_session_date;
    bool m_image_maximize_mode = false;

    ThumbnailModel *m_thumbnail_model = new ThumbnailModel();
    QLineEdit *m_thumbnail_search_edit = new QLineEdit();

    QWidget *m_right_pane = new QWidget();
    QWidget *m_left_pane = new QWidget();
    ThumbnailTools *m_thumbnail_tools_widget = new ThumbnailTools();
    QVBoxLayout *m_left_pane_layout = new QVBoxLayout();
    QVBoxLayout *m_right_pane_layout = new QVBoxLayout();
    NoteWidget *m_note_holder = new NoteWidget();
    QSplitter *m_right_pane_splitter = new QSplitter(Qt::Orientation::Vertical);
    bool m_auto_notes_popup = false;

    PixAnalyser *m_pix_analyser = nullptr;

    int m_slideshow_index = -1;
    QStringList m_slideshow_files;

    ManageTagDialog *m_manage_tag_dialog = nullptr;
    QStringList m_tags;
};


#endif
