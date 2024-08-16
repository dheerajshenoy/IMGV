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
    Thumbnail(const QString &filename, const QPixmap &pixmap) noexcept;
    Thumbnail(const QString &filename, const QString &note) noexcept;
    Thumbnail(const QString &filename, const QString &note, const QString &tag) noexcept;
    ~Thumbnail() {}

    enum Data {
        FileName = Qt::UserRole,
        Note,
        Tag
    };

    inline const QString& note() noexcept
    {
        return m_note;
    }

    inline QPixmap pixmap() noexcept
    {
        return m_pix;
    }

    inline const QString& filename() noexcept
    {
        return m_filename;
    }

    inline void setFilename(const QString &filename) noexcept
    {
        m_filename = filename;
    }

    inline void setNote(const QString &note) noexcept
    {
        if (!note.isEmpty()) m_note = note;
    }

    inline bool hasNote() noexcept
    {
        return !m_note.isEmpty();
    }

    inline void setTag(const QString tag) noexcept
    {
        m_tag = tag;
    }

    inline QString tag() noexcept
    {
        return m_tag;
    }


private:
    QPixmap m_pix;
    QString m_note;
    QString m_filename = "";
    QString m_tag;
};

#endif
