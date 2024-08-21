#include "ThumbnailModel.hpp"
ThumbnailModel::ThumbnailModel(QObject *parent)
    : QAbstractListModel(parent)
{}

int ThumbnailModel::addThumbnail(const Thumbnail &thumbnail) noexcept
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_thumbnails.push_back(thumbnail);
    endInsertRows();
    return m_thumbnails.size();
}

int ThumbnailModel::addThumbnails(const QVector<Thumbnail> &thumbnails) noexcept
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    for(const auto &thumbnail: thumbnails)
        m_thumbnails.push_back(thumbnail);
    endInsertRows();

    return m_thumbnails.size();
}

void ThumbnailModel::setNote(const QModelIndex &index, const QString &note) noexcept
{
    m_thumbnails[index.row()].setNote(note);
    emit dataChanged(index, index, {Thumbnail::Note});
}

void ThumbnailModel::setNote(const QModelIndex &index, QString &&note) noexcept
{
    m_thumbnails[index.row()].setNote(note);
    emit dataChanged(index, index, {Thumbnail::Note});
}

void ThumbnailModel::setTag(const QModelIndex &index, const QString &tagname) noexcept
{
    m_thumbnails[index.row()].setTag(tagname);
    emit dataChanged(index, index, {Thumbnail::Tag});
}

Thumbnail ThumbnailModel::getThumbnail(const int& index) const noexcept
{
    return m_thumbnails.at(index);
}

QVariant ThumbnailModel::data(const QModelIndex &index, int role) const noexcept
{
    if (!index.isValid() || index.row() >= rowCount())
        return QVariant();

    const Thumbnail &thumbnail = m_thumbnails.at(index.row());

    if (role == Thumbnail::FileName)
        return thumbnail.filename();
    else if (role == Qt::DisplayRole)
        return QFileInfo(thumbnail.filename()).fileName();
    else if (role == Qt::DecorationRole)
        return QIcon((thumbnail).pixmap());
    else if (role == Thumbnail::Note)
        return thumbnail.note();
    else if (role == Thumbnail::Tag)
        return thumbnail.tag();
    else if (role == Qt::ToolTipRole)
        return thumbnail.filename();

    return QVariant();
}

void ThumbnailModel::clear() noexcept
{
    beginResetModel();
    m_thumbnails.clear();
    endResetModel();
}

void ThumbnailModel::removeIndexes(const QList<QModelIndex> &indexes) noexcept
{
    beginRemoveRows(QModelIndex(), rowCount(), rowCount());
    for(int i=indexes.size() - 1; i >= 0; i--)
        this->removeAt(indexes.at(i).row());
    endRemoveRows();
}

QStringList ThumbnailModel::getFiles() const noexcept
{
    QStringList filenames;
    filenames.resize(m_thumbnails.size());
    for(int i=0; i < m_thumbnails.size(); i++)
        filenames[i] = m_thumbnails.at(i).filename();
    return filenames;
}
