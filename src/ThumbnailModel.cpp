#include "ThumbnailModel.hpp"
ThumbnailModel::ThumbnailModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

void ThumbnailModel::addThumbnail(const Thumbnail &thumbnail) {
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_thumbnails.append(thumbnail);
    endInsertRows();
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

Thumbnail ThumbnailModel::getThumbnail(const int index) noexcept
{
    return m_thumbnails[index];
}

QVariant ThumbnailModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= rowCount())
        return QVariant();

    const Thumbnail &thumbnail = m_thumbnails.at(index.row());
    if (role == Qt::UserRole)
        return const_cast<Thumbnail &>(thumbnail).filename();
    else if (role == Qt::DisplayRole)
        return QFileInfo(const_cast<Thumbnail &>(thumbnail).filename()).fileName();
    else if (role == Qt::DecorationRole)
        return QIcon(const_cast<Thumbnail&>(thumbnail).pixmap());
    else if (role == Thumbnail::Note)
        return const_cast<Thumbnail&>(thumbnail).note();

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
    m_thumbnails.removeAt(index);
}
