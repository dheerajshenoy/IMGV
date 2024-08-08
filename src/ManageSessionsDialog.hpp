#ifndef MANAGE_SESSIONS_DIALOG_HPP
#define MANAGE_SESSIONS_DIALOG_HPP

#include <qt6/QtWidgets/QDialog>
#include <qt6/QtWidgets/QVBoxLayout>
#include <qt6/QtWidgets/QMessageBox>
#include <qt6/QtWidgets/QPushButton>
#include <qt6/QtWidgets/QTableWidget>
#include <qt6/QtWidgets/QInputDialog>
#include <qt6/QtWidgets/QHeaderView>
#include <qt6/QtWidgets/QMenu>
#include <qt6/QtGui/QAction>
#include <qt6/QtGui/QDesktopServices>
#include <qt6/QtCore/QDir>
#include "utils.hpp"

class ManageSessionsDialog : public QDialog
{
public:
    ManageSessionsDialog(QString &sessionDirPath, QWidget *parent = nullptr);
    ~ManageSessionsDialog() {}

private:
    QVBoxLayout *layout = new QVBoxLayout();
    QTableWidget *table = new QTableWidget();
    QMenu *contextMenu = new QMenu();
    QAction *renameSession = new QAction("Rename Session");
    QAction *deleteSession = new QAction("Delete Session");
    QAction *openInExplorer = new QAction("Open in Explorer");
    QPushButton *done_btn = new QPushButton("Done");

    void RenameSession() noexcept;
    void OpenInExplorer() noexcept;
    void DeleteSession() noexcept;
    void showContextMenu(const QPointF loc) noexcept;
    QStringList getSessions() noexcept;


    QString session_path;
};

#endif
