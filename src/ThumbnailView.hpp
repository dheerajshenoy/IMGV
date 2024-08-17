#ifndef THUMBNAIL_VIEW_HPP
#define THUMBNAIL_VIEW_HPP

#include <QListView>
#include <QMessageBox>
#include <QMenu>
#include <QAction>
#include <QDragMoveEvent>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QDesktopServices>
#include <QStandardItem>
#include <QProcess>
#include <QHideEvent>
#include <QShowEvent>
#include "utils.hpp"
#include "ThumbnailModel.hpp"
#include "ThumbnailFilterProxy.hpp"
#include "Thumbnail.hpp"
#include "ImagePropertiesDialog.hpp"

class ThumbnailView : public QListView
{
    Q_OBJECT
public:
    ThumbnailView(QWidget *parent = nullptr);
    ThumbnailModel* model() const noexcept;
    void createThumbnail(const QString &fileName) noexcept;
    void createThumbnail(QString&& fileName) noexcept;
    void createThumbnails(const QStringList &fileNames) noexcept;
    void createThumbnails(const QList<Thumbnail> &thumbnails) noexcept;
    void loadFile(const QString &path) const noexcept;
    void gotoNext() noexcept;
    void gotoPrev() noexcept;
    int count() const noexcept;
    QString item(int&& index, int&& role) noexcept;
    QString item(const int& index, const int& role) noexcept;
    Thumbnail thumbnail(int&& index) const noexcept;
    Thumbnail thumbnail(const int& index) const noexcept;
    void clear() noexcept;
    void search(QString&&) const noexcept;
    void search(const QString&) const noexcept;
    void searchMode(const bool&) noexcept;
    void filterMode(const bool&) noexcept;
    void filter(const QString&) const noexcept;
    Thumbnail currentThumbnail() const noexcept;
    QString getFile(const int &index) const noexcept;
    const QStringList getAllFiles() const noexcept;
    QVector<Thumbnail>& getAllThumbnails() const noexcept;
    void setCurrentThumbnail(const int& index) noexcept;
    void setHighlightIndex(const int& row) noexcept;
    int currentHighlightIndex() const noexcept;
    void setFilterByTag(const QString& tagname) noexcept;

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

    int addThumbnail(const QString &filename) noexcept;
    ThumbnailModel *m_model = new ThumbnailModel();
    void showContextMenu(const QPoint &pos) const noexcept;
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
