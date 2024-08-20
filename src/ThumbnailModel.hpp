#ifndef THUMBNAIL_MODEL_HPP
#define THUMBNAIL_MODEL_HPP

#include <QAbstractListModel>
#include <QFile>
#include <QFileInfo>
#include <QVector>
#include "Thumbnail.hpp"
#include <algorithm>

class ThumbnailModel : public QAbstractListModel
{
public:



    enum Sort {
        Date,
        Size,
        Name,
    };

    ThumbnailModel(QObject *parent = nullptr);
    int addThumbnail(const Thumbnail &thumbnail) noexcept;
    int addThumbnails(const QVector<Thumbnail> &thumbnails) noexcept;
    /*int rowCount(const QModelIndex &parent = QModelIndex()) const noexcept override;*/
    QVariant data(const QModelIndex &index, int role) const noexcept override;
    void clear() noexcept;
    void sort(const Sort&& sort, const bool& desc = false) noexcept;
    void sort(const Sort& sort, const bool& desc = false) noexcept;

    inline void removeAt(const int &index) noexcept
    {
        m_thumbnails.remove(index);
    }

    inline void removeAt(const int&& index) noexcept
    {
        m_thumbnails.remove(index);
    }

    void removeIndexes(const QList<QModelIndex> &indexes) noexcept;
    Thumbnail getThumbnail(const int& index) const noexcept;
    void setNote(const QModelIndex &index, const QString &note) noexcept;
    void setNote(const QModelIndex &index, QString&& note) noexcept;
    void setTag(const QModelIndex &index, const QString &tag) noexcept;
    QStringList getFiles() const noexcept;

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
