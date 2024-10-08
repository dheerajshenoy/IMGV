#ifndef MANAGE_SESSIONS_DIALOG_HPP
#define MANAGE_SESSIONS_DIALOG_HPP

#include <QDialog>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QInputDialog>
#include <QHeaderView>
#include <QMenu>
#include <QAction>
#include <QDesktopServices>
#include <QDir>
#include "utils.hpp"

class ManageSessionsDialog : public QDialog
{
    Q_OBJECT
public:
    ManageSessionsDialog(const QString &sessionDirPath, QWidget *parent = nullptr);

signals:
    void openSession(QString);

private:

    void OpenSession() noexcept;
    void RenameSession() noexcept;
    void DeleteSession() const noexcept;
    QStringList getSessions() const noexcept;

    inline void showContextMenu(QPointF loc) const noexcept
    {
        contextMenu->exec(mapToGlobal(loc.toPoint()));
    }

    inline void OpenInExplorer() const noexcept
    {
        auto session = table->selectedItems()[0]->text();
        QDesktopServices::openUrl(session_path);
    }

    QVBoxLayout *layout = new QVBoxLayout();
    QTableWidget *table = new QTableWidget();
    QMenu *contextMenu = new QMenu();
    QAction *renameSession = new QAction("Rename Session");
    QAction *deleteSession = new QAction("Delete Session");
    QAction *openInExplorer = new QAction("Open in Explorer");
    QAction *_openSession = new QAction("Open Session");
    QPushButton *done_btn = new QPushButton("Done");
    QString session_path;

};

#endif
