#include "ThumbnailModel.hpp"
ThumbnailModel::ThumbnailModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int ThumbnailModel::addThumbnail(const Thumbnail &thumbnail) {
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_thumbnails.append(thumbnail);
    endInsertRows();
    return m_thumbnails.size();
}

int ThumbnailModel::addThumbnails(const QVector<Thumbnail> &thumbnails) {
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    for(const auto &thumbnail: thumbnails)
        m_thumbnails.append(thumbnail);
    endInsertRows();
    return m_thumbnails.size();
}


int ThumbnailModel::rowCount(const QModelIndex &parent) const
{
    return static_cast<int>(m_thumbnails.size());
}

void ThumbnailModel::setNote(const QModelIndex &index, const QString &note) noexcept
{
    m_thumbnails[index.row()].setNote(note);
    emit dataChanged(index, index, {Thumbnail::Note});
}

void ThumbnailModel::setTag(const QModelIndex &index, const QString &tagname) noexcept
{
    m_thumbnails[index.row()].setTag(tagname);
    emit dataChanged(index, index, {Thumbnail::Tag});
}

Thumbnail ThumbnailModel::getThumbnail(const int index) noexcept
{
    return m_thumbnails[index];
}

QVariant ThumbnailModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= rowCount())
        return QVariant();

    const Thumbnail &thumbnail = m_thumbnails.at(index.row());

    if (role == Thumbnail::FileName)
        return const_cast<Thumbnail &>(thumbnail).filename();
    else if (role == Qt::DisplayRole)
        return QFileInfo(const_cast<Thumbnail &>(thumbnail).filename()).fileName();
    else if (role == Qt::DecorationRole)
        return QIcon(const_cast<Thumbnail&>(thumbnail).pixmap());
    else if (role == Thumbnail::Note)
        return const_cast<Thumbnail&>(thumbnail).note();
    else if (role == Thumbnail::Tag)
        return const_cast<Thumbnail&>(thumbnail).tag();
    else if (role == Qt::ToolTipRole)
        return const_cast<Thumbnail &>(thumbnail).filename();

    return QVariant();
}

void ThumbnailModel::clear() noexcept
{
    beginResetModel();
    m_thumbnails.clear();
    endResetModel();
}

void ThumbnailModel::removeAt(const int index) noexcept
{
    m_thumbnails.remove(index);
}

void ThumbnailModel::removeIndexes(const QList<QModelIndex> &indexes) noexcept
{
    beginRemoveRows(QModelIndex(), rowCount(), rowCount());
    for(int i=indexes.size() - 1; i >= 0; i--)
        this->removeAt(indexes.at(i).row());
    endRemoveRows();
}

QStringList ThumbnailModel::getFiles() noexcept
{
    QStringList filenames;
    filenames.resize(m_thumbnails.size());
    for(int i=0; i < m_thumbnails.size(); i++)
        filenames[i] = m_thumbnails[i].filename();
    return filenames;
}


