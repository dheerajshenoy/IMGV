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

    return QVariant();
}

void ThumbnailModel::clear() noexcept
{
    beginResetModel();
    m_thumbnails.clear();
    endResetModel();
}

void ThumbnailModel::removeAt(int index) noexcept
{
    m_thumbnails.removeAt(index);
}
