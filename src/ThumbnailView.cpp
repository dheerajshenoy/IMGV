#include "ThumbnailView.hpp"

ThumbnailView::ThumbnailView(QWidget *parent)
    : QListView(parent)
{
    setWindowIcon(QIcon(":/icons/imgv.png"));
    setViewMode(QListView::IconMode);
    setIconSize(QSize(100, 100));  // Set the size for thumbnails
    setResizeMode(QListView::Adjust);
    setSpacing(20);  // Spacing between thumbnails
    setTextElideMode(Qt::TextElideMode::ElideRight);
    setWordWrap(true);
    setSelectionMode(SelectionMode::ExtendedSelection);
    /*this->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);*/
    /*setUniformItemSizes(true);*/

    this->setContextMenuPolicy(Qt::CustomContextMenu);
    this->setModel(m_model);

    connect(this, &ThumbnailView::customContextMenuRequested, this, &ThumbnailView::showContextMenu);

    m_contextMenu->addAction(m_action__remove);
    m_contextMenu->addAction(m_action__hide);
    m_contextMenu->addAction(m_action__show_in_explorer);
    m_contextMenu->addAction(m_action__select_all);
    m_contextMenu->addAction(m_action__image_properties);

    connect(m_action__select_all, &QAction::triggered, this, [&]() {
        this->selectAll();
    });

    connect(m_action__image_properties, &QAction::triggered, this, &ThumbnailView::showProperties);

    m_partialContextMenu->addAction(m_action__select_all);

    connect(m_action__remove, &QAction::triggered, this, &ThumbnailView::removeThumbnails);
    connect(m_action__hide, &QAction::triggered, this, &ThumbnailView::hideThumbnails);
    connect(m_action__show_in_explorer, &QAction::triggered, this, &ThumbnailView::showFilesInExplorer);

    m_filter_proxy->setSourceModel(m_model);
}

void ThumbnailView::loadFile(const QString &file) noexcept
{
    Thumbnail thumb(file);
    m_model->addThumbnail(thumb);
}

ThumbnailModel* ThumbnailView::model() noexcept
{
    return m_model;
}

void ThumbnailView::createThumbnails(const QStringList &fileNames) noexcept
{
    for (const QString &fileName : fileNames) {
        Thumbnail thumb(fileName);
        m_model->addThumbnail(thumb);
    }

    if (m_model->rowCount() >= 0)
        setCurrentIndex(m_model->index(0));
}

void ThumbnailView::createThumbnails(const QList<Thumbnail> &thumbnails) noexcept
{
    for (const Thumbnail &thumb : thumbnails) {
        m_model->addThumbnail(thumb);
    }

    if (m_model->rowCount() >= 0)
        setCurrentIndex(m_model->index(0));
}

void ThumbnailView::dragEnterEvent(QDragEnterEvent *e)
{
    const QMimeData *mimedata = e->mimeData();
    if (mimedata->hasUrls())
        e->acceptProposedAction();
}

void ThumbnailView::dragLeaveEvent(QDragLeaveEvent *e)
{
    e->accept();
}

void ThumbnailView::dragMoveEvent(QDragMoveEvent *e)
{
    e->accept();
    e->acceptProposedAction();
}

void ThumbnailView::dropEvent(QDropEvent *e)
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
                m_model->addThumbnail(filepath);
            }
        }
    }
    e->acceptProposedAction();
}

void ThumbnailView::showContextMenu(const QPoint &pos) noexcept
{
    if (this->selectedIndexes().size() > 0 || this->indexAt(pos).isValid())
        m_contextMenu->exec(mapToGlobal(pos));
    else
        m_partialContextMenu->exec(mapToGlobal(pos));

}

void ThumbnailView::removeThumbnails() noexcept
{
    auto selections = this->selectedIndexes();
    for(const auto index : selections)
    {
        m_model->removeAt(index.row());
    }
}

void ThumbnailView::hideThumbnails() noexcept
{
    auto selections = this->selectedIndexes();
    for(const auto &s : selections)
        this->setRowHidden(s.row(), true);
}

void ThumbnailView::showFilesInExplorer() noexcept
{
    auto selection = this->selectedIndexes()[0];
    auto filepath = QFileInfo(selection.data(Qt::UserRole).toString()).path();
    QDesktopServices::openUrl(QUrl::fromLocalFile(filepath));
}

void ThumbnailView::gotoNext() noexcept
{
    if (currentIndex().row() < m_model->rowCount() - 1)
    {
        this->setCurrentIndex(currentIndex().siblingAtRow(currentIndex().row() + 1));
        emit fileChangeRequested(currentIndex().data(Qt::UserRole).toString());
    }
}

void ThumbnailView::gotoPrev() noexcept
{
    if (currentIndex().row() > 0)
    {
        this->setCurrentIndex(currentIndex().siblingAtRow(currentIndex().row() - 1));
        emit fileChangeRequested(currentIndex().data(Qt::UserRole).toString());
    }
}

void ThumbnailView::hideEvent(QHideEvent *e) noexcept
{
    emit visibilityChanged(isVisible());
}

void ThumbnailView::showEvent(QShowEvent *e) noexcept
{
    emit visibilityChanged(isVisible());
}

void ThumbnailView::addThumbnail(const QString &filename) noexcept
{
    m_model->addThumbnail(filename);
}

int ThumbnailView::count() noexcept
{
    return m_model->rowCount();
}

QString ThumbnailView::item(int _index, int role) noexcept
{
    auto index = this->currentIndex().siblingAtRow(_index);
    return m_model->data(index, role).toString();
}

void ThumbnailView::search(QString text) noexcept
{
    m_filter_proxy->setFilterText(text);
}

void ThumbnailView::searchMode(bool state) noexcept
{
    if (state)
        this->setModel(m_filter_proxy);
    else
        this->setModel(m_model);
}

Thumbnail ThumbnailView::currentThumbnail() noexcept
{
    return m_model->getThumbnail(currentIndex().row());
}

QString ThumbnailView::getFile(const int index) noexcept
{
    return m_model->index(index).data(Qt::UserRole).toString();
}

void ThumbnailView::showProperties() noexcept
{
    ImagePropertiesDialog *pd = new ImagePropertiesDialog(getFile(currentIndex().row()), this);
    pd->open();
}

const QStringList ThumbnailView::getAllFiles() noexcept
{
    return m_model->getFiles();
}
