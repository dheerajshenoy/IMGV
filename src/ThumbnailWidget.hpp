#ifndef THUMBNAIL_WIDGET_HPP
#define THUMBANIL_WIDGET_HPP

#include <qt6/QtWidgets/QListWidget>
#include <qt6/QtWidgets/QMenu>
#include <qt6/QtGui/QAction>
#include <qt6/QtGui/QDragMoveEvent>
#include <qt6/QtGui/QDragEnterEvent>
#include <qt6/QtGui/QDragLeaveEvent>
#include <qt6/QtGui/QDropEvent>
#include <qt6/QtCore/QMimeData>
#include <qt6/QtGui/QDesktopServices>
#include <qt6/QtCore/QProcess>
#include "Thumbnail.hpp"
#include "utils.hpp"

class ThumbnailWidget : public QListWidget
{
    Q_OBJECT
public:
    explicit ThumbnailWidget(QWidget *parent = nullptr);
    ~ThumbnailWidget(){}

    void createThumbnails(const QStringList &fileNames);
    void addThumbnail(const QString &filename);
    void gotoNext();
    void gotoPrev();

signals:
    void fileChangeRequested(QString);

protected:
    void dragEnterEvent(QDragEnterEvent *e) override;
    void dragLeaveEvent(QDragLeaveEvent *e) override;
    void dragMoveEvent(QDragMoveEvent *e) override;
    void dropEvent(QDropEvent *e) override;

private:
    QPixmap createThumbnail(const QString &fileName, const QSize &size);
    void showContextMenu(const QPoint &pos) noexcept;
    void removeThumbnails();
    void hideThumbnails();
    void showFilesInExplorer();
    void showProperties();

    QMenu *m_contextMenu = new QMenu("Context Menu", this);
    QMenu *m_partialContextMenu = new QMenu("Partial Menu", this);
    QAction *m_action__select_all = new QAction("Select All");
    QAction *m_action__remove = new QAction("Remove");
    QAction *m_action__hide = new QAction("Hide");
    QAction *m_action__show_in_explorer = new QAction("Show in File Explorer");
    QAction *m_action__image_properties = new QAction("Properties");
};

#endif
