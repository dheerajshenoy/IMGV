#ifndef THUMBNAIL_MODEL_HPP
#define THUMBNAIL_MODEL_HPP

#include <QAbstractListModel>
#include <QFile>
#include <QFileInfo>
#include <QVector>
#include "Thumbnail.hpp"

class ThumbnailModel : public QAbstractListModel
{
    Q_OBJECT
public:

    ThumbnailModel(QObject *parent = nullptr);

    void addThumbnail(const Thumbnail &thumbnail);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    void clear() noexcept;
    void removeAt(const int index) noexcept;
    Thumbnail getThumbnail(const int index) noexcept;
    void setNote(const QModelIndex &index, const QString &note) noexcept;

private:
    QVector<Thumbnail> m_thumbnails;

};

#endif
