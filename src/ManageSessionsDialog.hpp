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
    ManageSessionsDialog(QString &sessionDirPath, QWidget *parent = nullptr);
    ~ManageSessionsDialog() {}

signals:
    void openSession(QString);

private:
    QVBoxLayout *layout = new QVBoxLayout();
    QTableWidget *table = new QTableWidget();
    QMenu *contextMenu = new QMenu();
    QAction *renameSession = new QAction("Rename Session");
    QAction *deleteSession = new QAction("Delete Session");
    QAction *openInExplorer = new QAction("Open in Explorer");
    QAction *_openSession = new QAction("Open Session");
    QPushButton *done_btn = new QPushButton("Done");

    void OpenSession() noexcept;
    void RenameSession() noexcept;
    void OpenInExplorer() noexcept;
    void DeleteSession() noexcept;
    void showContextMenu(const QPointF loc) noexcept;
    QStringList getSessions() noexcept;

    QString session_path;
};

#endif
