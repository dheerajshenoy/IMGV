#ifndef THUMBNAIL_VIEW_HPP
#define THUMBNAIL_VIEW_HPP

#include <qt6/QtWidgets/QListView>
#include <qt6/QtWidgets/QMessageBox>
#include <qt6/QtWidgets/QMenu>
#include <qt6/QtGui/QAction>
#include <qt6/QtGui/QDragMoveEvent>
#include <qt6/QtGui/QDragEnterEvent>
#include <qt6/QtGui/QDragLeaveEvent>
#include <qt6/QtGui/QDropEvent>
#include <qt6/QtCore/QMimeData>
#include <qt6/QtGui/QDesktopServices>
#include <qt6/QtGui/QStandardItem>
#include <qt6/QtCore/QProcess>
#include <qt6/QtGui/QHideEvent>
#include <qt6/QtGui/QShowEvent>
#include "utils.hpp"
#include "ThumbnailModel.hpp"
#include "ThumbnailFilterProxy.hpp"
#include "ImagePropertiesDialog.hpp"

class ThumbnailView : public QListView
{
    Q_OBJECT
public:
    ThumbnailView(QWidget *parent = nullptr);
    ThumbnailModel* model() noexcept;
    void createThumbnails(const QStringList &fileNames) noexcept;
    void createThumbnails(const QList<Thumbnail> &thumbnails) noexcept;
    void addThumbnail(const QString &filename) noexcept;
    void loadFile(const QString &path) noexcept;
    void gotoNext() noexcept;
    void gotoPrev() noexcept;
    int count() noexcept;
    QString item(int index, int role) noexcept;
    void clear() noexcept;
    void search(QString) noexcept;
    void searchMode(bool) noexcept;
    Thumbnail currentThumbnail() noexcept;
    QString getFile(const int index) noexcept;

signals:
    void fileChangeRequested(QString);
    void visibilityChanged(bool);

protected:
    void dragEnterEvent(QDragEnterEvent *e) override;
    void dragLeaveEvent(QDragLeaveEvent *e) override;
    void dragMoveEvent(QDragMoveEvent *e) override;
    void dropEvent(QDropEvent *e) override;
    void hideEvent(QHideEvent *e) noexcept override;
    void showEvent(QShowEvent *e) noexcept override;

private:

    ThumbnailModel *m_model = new ThumbnailModel();
    void showContextMenu(const QPoint &pos) noexcept;
    void removeThumbnails() noexcept;
    void hideThumbnails() noexcept;
    void showFilesInExplorer() noexcept;
    void showProperties() noexcept;

    QMenu *m_contextMenu = new QMenu("Context Menu", this);
    QMenu *m_partialContextMenu = new QMenu("Partial Menu", this);
    QAction *m_action__select_all = new QAction("Select All");
    QAction *m_action__remove = new QAction("Remove");
    QAction *m_action__hide = new QAction("Hide");
    QAction *m_action__show_in_explorer = new QAction("Show in File Explorer");
    QAction *m_action__image_properties = new QAction("Properties");

    ThumbnailFilterProxy *m_filter_proxy = new ThumbnailFilterProxy();
};

#endif
