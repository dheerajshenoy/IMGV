#include "ManageSessionsDialog.hpp"

ManageSessionsDialog::ManageSessionsDialog(QString &sessionDirPath, QWidget *parent)
    : QDialog(parent), session_path(sessionDirPath)
{
    table->setColumnCount(2);
    table->setHorizontalHeaderLabels({ "Session Name", "Images" });

    auto files = QDir(sessionDirPath).entryList(QStringList() << "*.imgv" << "*.IMGV", QDir::Files);
    table->setRowCount(files.size());

    table->horizontalHeader()->setStretchLastSection(true);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Stretch);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);

    for(int i=0; i < files.size(); i++)
    {
        QTableWidgetItem *session_file = new QTableWidgetItem(QFileInfo(files[i]).baseName());
        QStringList imgfiles = utils::getImagesFromSessionFile(QString("%1%2%3").arg(sessionDirPath).arg(QDir::separator()).arg(files[i]));
        QTableWidgetItem *img_count = new QTableWidgetItem(QString::number(imgfiles.size()));
        table->setItem(i, 0, session_file);
        table->setItem(i, 1, img_count);
    }

    layout->addWidget(table);
    this->setLayout(layout);

    layout->addWidget(done_btn);

    connect(done_btn, &QPushButton::clicked, this, &QDialog::done);

    contextMenu->addAction(renameSession);
    contextMenu->addAction(deleteSession);
    contextMenu->addAction(openInExplorer);

    table->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(renameSession, &QAction::triggered, this, &ManageSessionsDialog::RenameSession);
    connect(deleteSession, &QAction::triggered, this, &ManageSessionsDialog::DeleteSession);
    connect(openInExplorer, &QAction::triggered, this, &ManageSessionsDialog::OpenInExplorer);
    connect(table, &QTableWidget::customContextMenuRequested, this, &ManageSessionsDialog::showContextMenu);
}

void ManageSessionsDialog::showContextMenu(const QPointF loc) noexcept
{
    contextMenu->exec(mapToGlobal(loc.toPoint()));
}

void ManageSessionsDialog::DeleteSession() noexcept
{
    auto sessions = table->selectedItems();
    QMessageBox msgbox;

    if (sessions.count() / 2 > 1)
        msgbox.setStandardButtons(QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::No | QMessageBox::StandardButton::YesToAll |
                                  QMessageBox::StandardButton::NoToAll);
    else
        msgbox.setStandardButtons(QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::No);
    msgbox.setWindowTitle("Deleting Session");
    bool check = true;
    for(int i=0; i < sessions.count(); i += 2)
    {
        auto session = sessions[i];
        auto filepath = QString("%1%2%3").arg(session_path).arg(QDir::separator()).arg(session->text() + ".imgv");
        if (check)
        {
            msgbox.setText(QString("Are you sure about deleting %1 ?").arg(session->text()));
            switch(msgbox.exec())
            {
                case QMessageBox::Yes:
                    QFile(filepath).remove();
                    table->removeRow(session->row());
                    break;

                case QMessageBox::No:
                    break;

                case QMessageBox::YesToAll:
                    check = false;
                    QFile(filepath).remove();
                    table->removeRow(session->row());
                    break;

                case QMessageBox::NoToAll:
                    return;
            }
        }
        else
        {
            QFile(filepath).remove();
            table->removeRow(session->row());
        }
    }
}

QStringList ManageSessionsDialog::getSessions() noexcept
{
    return QDir(session_path).entryList(QStringList() << "*.imgv" << "*.IMGV", QDir::Files);
}

void ManageSessionsDialog::RenameSession() noexcept
{
    auto index = table->selectedItems()[0];
    auto session = index->text();

    QString new_sess_name = QInputDialog::getText(this, "Rename Session", QString("Enter a new name for the `%1` session").arg(session));

    // Cancel if user exits the input dialog or if empty
    if (new_sess_name.isEmpty()) return;

    if (getSessions().indexOf(new_sess_name + ".imgv") >= 0)
    {
        QMessageBox::critical(this, "Error Renaming Session", "The new session name you specified already exists. Please try again with a new name");
        return;
    }

    QFile file(QString("%1%2%3").arg(session_path).arg(QDir::separator()).arg(session + ".imgv"));

    if (!file.rename(session_path + QDir::separator() + new_sess_name + ".imgv"))
        QMessageBox::critical(this, "Rename Unsuccessful", QString("%1 could not be renamed due to some reason.").arg(session));
}

void ManageSessionsDialog::OpenInExplorer() noexcept
{
    auto session = table->selectedItems()[0]->text();
    QDesktopServices::openUrl(session_path);
}
