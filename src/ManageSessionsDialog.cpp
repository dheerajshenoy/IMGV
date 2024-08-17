#include "ManageSessionsDialog.hpp"

ManageSessionsDialog::ManageSessionsDialog(const QString &sessionDirPath, QWidget *parent)
    : QDialog(parent), session_path(sessionDirPath)
{
    table->setColumnCount(4);
    table->setHorizontalHeaderLabels({ "Session Name", "Date Modified", "Images", "Tags" });

    auto files = QDir(session_path).entryList(QStringList() << "*.imgv" << "*.IMGV", QDir::Files);
    table->setRowCount(files.size());

    table->horizontalHeader()->setStretchLastSection(true);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Stretch);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);

    for(int i=0; i < files.size(); i++)
    {
        QTableWidgetItem *session_file = new QTableWidgetItem(QFileInfo(files[i]).baseName());
        Custom file = utils::getInfoFromSessionFile(QString("%1%2%3").arg(session_path).arg(QDir::separator()).arg(files[i]));
        auto imgfiles = file.files;
        auto tags = file.tags;
        QTableWidgetItem *img_count = new QTableWidgetItem(QString::number(imgfiles.size()));
        QTableWidgetItem *date = new QTableWidgetItem(file.date);
        QString _tag = tags.join(",");
        QTableWidgetItem *tagitem = new QTableWidgetItem(_tag);

        img_count->setToolTip(imgfiles.join("\n"));

        table->setItem(i, 0, session_file);
        table->setItem(i, 1, date);
        table->setItem(i, 2, img_count);
        table->setItem(i, 3, tagitem);
    }

    layout->addWidget(table);
    this->setLayout(layout);

    layout->addWidget(done_btn);

    connect(done_btn, &QPushButton::clicked, this, &QDialog::done);

    contextMenu->addAction(renameSession);
    contextMenu->addAction(deleteSession);
    contextMenu->addAction(openInExplorer);
    contextMenu->addAction(_openSession);

    table->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(renameSession, &QAction::triggered, this, &ManageSessionsDialog::RenameSession);
    connect(deleteSession, &QAction::triggered, this, &ManageSessionsDialog::DeleteSession);
    connect(openInExplorer, &QAction::triggered, this, &ManageSessionsDialog::OpenInExplorer);
    connect(_openSession, &QAction::triggered, this, &ManageSessionsDialog::OpenSession);
    connect(table, &QTableWidget::customContextMenuRequested, this, &ManageSessionsDialog::showContextMenu);
}

void ManageSessionsDialog::OpenSession() noexcept
{
    auto sessions = table->selectedItems();

    for(int i=0; i < sessions.count() / 2; i += 2)
        emit openSession(session_path + QDir::separator() + sessions[i]->text() + ".imgv");
}

void ManageSessionsDialog::DeleteSession() const noexcept
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
    for(int i=0; i < sessions.count() / 2; i += 2)
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

QStringList ManageSessionsDialog::getSessions() const noexcept
{
    return QDir().entryList(QStringList() << "*.imgv" << "*.IMGV", QDir::Files);
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
