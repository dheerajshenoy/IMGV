#ifndef THUMBNAIL_HPP
#define THUMBNAIL_HPP

#include <QWidget>
#include <QPixmap>
#include "utils.hpp"

class Thumbnail
{
public:
    Thumbnail();
    Thumbnail(const QString &filename) noexcept;
    Thumbnail(const QString &filename, const QString &note) noexcept;
    ~Thumbnail() {}

    void setFilename(const QString &fileName) noexcept;
    void setNote(const QString &note) noexcept;
    const QString& note() noexcept;
    bool hasNote() noexcept;
    const QString& filename() noexcept;
    QPixmap pixmap() noexcept;

    enum Data {
        FileName = Qt::UserRole + 1,
        Note
    };

private:
    QPixmap m_pix;
    QString m_note;
    QString m_filename = "";
};

#endif
