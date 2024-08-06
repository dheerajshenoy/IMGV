#include "ThumbnailWidget.hpp"
#include "qnamespace.h"

ThumbnailWidget::ThumbnailWidget(QWidget *parent)
    : QListWidget(parent)
{
    
    setViewMode(QListWidget::IconMode);
    setIconSize(QSize(100, 100));  // Set the size for thumbnails
    setResizeMode(QListWidget::Adjust);
    setSpacing(20);  // Spacing between thumbnails
    setTextElideMode(Qt::TextElideMode::ElideRight);
    setWordWrap(true);
    setSelectionMode(SelectionMode::ExtendedSelection);
    /*setUniformItemSizes(true);*/

    this->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(this, &ThumbnailWidget::customContextMenuRequested, this, &ThumbnailWidget::showContextMenu);

    m_contextMenu->addAction(m_action__remove);
    m_contextMenu->addAction(m_action__hide);
    m_contextMenu->addAction(m_action__show_in_explorer);
    m_contextMenu->addAction(m_action__select_all);
    m_contextMenu->addAction(m_action__image_properties);

    connect(m_action__select_all, &QAction::triggered, this, [&]() {
        this->selectAll();
    });

    connect(m_action__image_properties, &QAction::triggered, this, &ThumbnailWidget::showProperties);

    m_partialContextMenu->addAction(m_action__select_all);

    connect(m_action__remove, &QAction::triggered, this, &ThumbnailWidget::removeThumbnails);
    connect(m_action__hide, &QAction::triggered, this, &ThumbnailWidget::hideThumbnails);
    connect(m_action__show_in_explorer, &QAction::triggered, this, &ThumbnailWidget::showFilesInExplorer);
}

void ThumbnailWidget::createThumbnails(const QStringList &fileNames)
{
    for (const QString &fileName : fileNames) {
        QPixmap thumbnail = createThumbnail(fileName, QSize(100, 100));
        if (!thumbnail.isNull()) {
            QListWidgetItem *item = new QListWidgetItem(QIcon(thumbnail), QFileInfo(fileName).fileName());
            item->setData(Qt::UserRole, fileName); // Store full file path in item
            addItem(item);
        } else {
            qWarning("Failed to create thumbnail for %s", qPrintable(fileName));
        }
    }
}

QPixmap ThumbnailWidget::createThumbnail(const QString &fileName, const QSize &size)
{
    QImage image(fileName);
    if (image.isNull()) {
        return QPixmap();  // Return an empty pixmap if the image fails to load
    }

    QPixmap pixmap = QPixmap::fromImage(image);
    return pixmap.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}


void ThumbnailWidget::dragEnterEvent(QDragEnterEvent *e)
{
    const QMimeData *mimedata = e->mimeData();

    if (mimedata->hasUrls())
        e->acceptProposedAction();

}

void ThumbnailWidget::dragLeaveEvent(QDragLeaveEvent *e)
{
    e->accept();
}

void ThumbnailWidget::dragMoveEvent(QDragMoveEvent *e)
{
    e->accept();
    e->acceptProposedAction();
}

void ThumbnailWidget::dropEvent(QDropEvent *e)
{
    /*if (e->source() == this) return;*/
    if (e->mimeData()->hasUrls())
    {
        QList<QUrl> urllist = e->mimeData()->urls();
        foreach(QUrl url, urllist)
        {
            if (url.isLocalFile())
            {
                QString filepath = url.toLocalFile();
                addThumbnail(filepath);
            }
        }
    }
    e->acceptProposedAction();
}

void ThumbnailWidget::addThumbnail(const QString &fileName)
{
    QPixmap thumbnail = createThumbnail(fileName, QSize(100, 100));
    if (!thumbnail.isNull()) {
        QListWidgetItem *item = new QListWidgetItem(QIcon(thumbnail), QFileInfo(fileName).fileName());
        item->setData(Qt::UserRole, fileName); // Store full file path in item
        addItem(item);
    } else {
        qWarning("Failed to create thumbnail for %s", qPrintable(fileName));
    }
}

void ThumbnailWidget::showContextMenu(const QPoint &pos) noexcept
{
    if (this->selectedItems().size() > 0 || this->itemAt(pos))
        m_contextMenu->exec(mapToGlobal(pos));
    else
        m_partialContextMenu->exec(mapToGlobal(pos));
}

void ThumbnailWidget::removeThumbnails()
{
    auto selections = this->selectedItems();
    for(const QListWidgetItem *s : selections)
    {
        delete this->takeItem(this->row(s));
    }
}

void ThumbnailWidget::hideThumbnails()
{

    auto selections = this->selectedItems();
    for(const auto &s : selections)
    {
        s->setHidden(true);
    }

}

void ThumbnailWidget::showFilesInExplorer()
{
    auto selections = this->selectedItems();

    for(const auto &s : selections)
    {
        auto filepath = QFileInfo(s->data(Qt::UserRole).toString()).path();
        QDesktopServices::openUrl(QUrl::fromLocalFile(filepath));
    }
}

void ThumbnailWidget::gotoNext()
{
    if (currentRow() < count() - 1)
    {
        this->setCurrentRow(currentRow() + 1);
        emit fileChangeRequested(currentItem()->data(Qt::UserRole).toString());
    }
}

void ThumbnailWidget::gotoPrev()
{
    if (currentRow() > 0)
    {
        this->setCurrentRow(currentRow() - 1);
        emit fileChangeRequested(currentItem()->data(Qt::UserRole).toString());
    }
}

void ThumbnailWidget::showProperties()
{
    for(const auto &item: this->selectedItems())
    {
        QString filepath = item->data(Qt::UserRole).toString();

    }
}
