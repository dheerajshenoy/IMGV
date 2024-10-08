#include "ThumbnailView.hpp"
#include <mutex>

ThumbnailView::ThumbnailView(QWidget *parent)
    : QListView(parent)
{

    setMovement(Movement::Snap);
    setWindowIcon(QIcon(":/icons/imgv.png"));

    this->setContextMenuPolicy(Qt::CustomContextMenu);
    this->setModel(m_model);

    /*setItemDelegate(m_item_delegate);*/
    setWordWrap(true);
    setIconSize(QSize(500, 500));

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

ThumbnailModel* ThumbnailView::model() const noexcept
{
    return m_model;
}

void ThumbnailView::createThumbnails(const QStringList &fileNames) noexcept
{
    QVector<Thumbnail> thumbnails;
    thumbnails.reserve(fileNames.size());
    for(int i=0; i < fileNames.size(); i++)
        thumbnails.emplace_back(fileNames.at(i));

    setUpdatesEnabled(false);
    m_model->addThumbnails(thumbnails);
    setUpdatesEnabled(true);

    if (m_model->rowCount() >= 0)
        setCurrentIndex(m_model->index(0));

}

void ThumbnailView::createThumbnail(const Thumbnail &thumb) noexcept
{
    m_model->addThumbnail(thumb);
    if (m_model->rowCount() >= 0)
        setCurrentIndex(m_model->index(0));
}

void ThumbnailView::createThumbnail(const QString &fileName) noexcept
{
    Thumbnail thumb(fileName);
    m_model->addThumbnail(thumb);
    if (m_model->rowCount() >= 0)
        setCurrentIndex(m_model->index(0));
}

void ThumbnailView::createThumbnail(QString&& fileName) noexcept
{
    Thumbnail thumb(fileName);
    
    m_model->addThumbnail(thumb);
    if (m_model->rowCount() >= 0)
        setCurrentIndex(m_model->index(0));
}

void ThumbnailView::createThumbnails(const QList<Thumbnail> &thumbnails) noexcept
{
    setUpdatesEnabled(false);

    for(const auto thumb : thumbnails)
        m_model->addThumbnail(thumb);

    if (m_model->rowCount() >= 0)
        setCurrentIndex(m_model->index(0));
    setUpdatesEnabled(true);
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

void ThumbnailView::showContextMenu(const QPoint &pos) const noexcept
{
    if (this->selectedIndexes().size() > 0 || this->indexAt(pos).isValid())
        m_contextMenu->exec(mapToGlobal(pos));
    else
        m_partialContextMenu->exec(mapToGlobal(pos));

}

void ThumbnailView::removeThumbnails() noexcept
{
    auto selections = this->selectedIndexes();
    // Loop in reverse, to avoid erros due to shifting of elements
    /*for (int i = selections.size() - 1; i >= 0; --i)*/
    /*    m_model->removeAt(selections.at(i).row());*/
    m_model->removeIndexes(selections);
    this->clearSelection();
    if (m_model->rowCount() == 0)
        this->setCurrentIndex(QModelIndex());
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

int ThumbnailView::addThumbnail(const QString &filename) noexcept
{
    int i = m_model->addThumbnail(filename);
    if (m_model->rowCount() == 0)
        setCurrentIndex(m_model->index(0));
    return i;
}

int ThumbnailView::count() const noexcept
{
    return m_model->rowCount();
}

QString ThumbnailView::item(int&& _index, int&& role) noexcept
{
    auto index = this->currentIndex().siblingAtRow(_index);
    return m_model->data(index, role).toString();
}

QString ThumbnailView::item(const int& _index, const int& role) noexcept
{
    auto index = this->currentIndex().siblingAtRow(_index);
    return m_model->data(index, role).toString();
}

void ThumbnailView::search(QString&& text) const noexcept
{
    m_filter_proxy->setSearchRole(Thumbnail::FileName);
    m_filter_proxy->setFilterText(text);
}

void ThumbnailView::search(const QString& text) const noexcept
{
    m_filter_proxy->setSearchRole(Thumbnail::FileName);
    m_filter_proxy->setFilterText(text);
}

void ThumbnailView::searchMode(const bool &state) noexcept
{
    if (state)
        this->setModel(m_filter_proxy);
    else
        this->setModel(m_model);
}

void ThumbnailView::filterMode(const bool &state) noexcept
{
    if (state)
        this->setModel(m_filter_proxy);
    else
        this->setModel(m_model);
}

void ThumbnailView::filter(const QString& tag) const noexcept
{
    m_filter_proxy->setSearchRole(Thumbnail::Tag);
    m_filter_proxy->setFilterText(tag);
}

Thumbnail ThumbnailView::currentThumbnail() const noexcept
{
    return m_model->getThumbnail(currentIndex().row());
}

QString ThumbnailView::getFile(const int& index) const noexcept
{
    return m_model->index(index).data(Qt::UserRole).toString();
}

void ThumbnailView::showProperties() noexcept
{
    ImagePropertiesDialog *pd = new ImagePropertiesDialog(getFile(currentIndex().row()), this);
    pd->open();
}

const QStringList ThumbnailView::getAllFiles() const noexcept
{
    return m_model->getFiles();
}

void ThumbnailView::setCurrentThumbnail(const int& index) noexcept
{


}

void ThumbnailView::setHighlightIndex(const int& row) noexcept
{
    setCurrentIndex(currentIndex().siblingAtRow(row));
}

int ThumbnailView::currentHighlightIndex() const noexcept
{
    return currentIndex().row();
}

Thumbnail ThumbnailView::thumbnail(int&& index) const noexcept
{
    if (index > m_model->rowCount() || index < 0)
        return Thumbnail();

    return m_model->getThumbnail(index);
}

Thumbnail ThumbnailView::thumbnail(const int& index) const noexcept
{
    if (index > m_model->rowCount() || index < 0)
        return Thumbnail();

    return m_model->getThumbnail(index);
}

QVector<Thumbnail>& ThumbnailView::getAllThumbnails() const noexcept
{
    return m_model->thumbnails();
}

void ThumbnailView::sort(const Sort& mode, const bool& desc) noexcept
{
    if (m_model->thumbnails().isEmpty())
        return;
}

void ThumbnailView::sort(const Sort&& mode, const bool&& desc) noexcept
{
    if (m_model->thumbnails().isEmpty())
        return;

    switch(mode)
    {
        case Sort::Name:
            m_filter_proxy->sort(0);
        break;

        case Sort::Size:
        break;

        case Sort::Date:
        break;
    }
    this->setModel(m_filter_proxy);
}
