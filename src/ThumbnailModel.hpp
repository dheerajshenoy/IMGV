#ifndef THUMBNAIL_MODEL_HPP
#define THUMBNAIL_MODEL_HPP

#include <QAbstractListModel>
#include <QFile>
#include <QFileInfo>
#include <QVector>
#include "Thumbnail.hpp"

class ThumbnailModel : public QAbstractListModel
{
public:

    ThumbnailModel(QObject *parent = nullptr);

    int addThumbnail(const Thumbnail &thumbnail) noexcept;
    int addThumbnails(const QVector<Thumbnail> &thumbnails) noexcept;
    /*int rowCount(const QModelIndex &parent = QModelIndex()) const noexcept override;*/
    QVariant data(const QModelIndex &index, int role) const noexcept override;
    void clear() noexcept;

    inline void removeAt(const int index) noexcept
    {
        m_thumbnails.remove(index);
    }

    void removeIndexes(const QList<QModelIndex> &indexes) noexcept;
    Thumbnail getThumbnail(const int index) noexcept;
    void setNote(const QModelIndex &index, const QString &note) noexcept;
    void setTag(const QModelIndex &index, const QString &tag) noexcept;
    QStringList getFiles() noexcept;

    inline QVector<Thumbnail>& thumbnails() noexcept
    {
        return m_thumbnails;
    }

    inline int rowCount(const QModelIndex &parent = QModelIndex()) const noexcept override
    {
        return static_cast<int>(m_thumbnails.size());
    }

private:
    QVector<Thumbnail> m_thumbnails;
};

#endif
