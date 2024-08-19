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
    void setThumbnailSize(const QSize& size) noexcept;

    enum Data {
        FileName = Qt::UserRole,
        Note,
        Tag
    };

    inline const QString& note() const noexcept
    {
        return m_note;
    }

    inline const QPixmap& pixmap() const noexcept
    {
        return m_pix;
    }

    inline const QString& filename() const noexcept
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

    inline bool hasNote() const noexcept
    {
        return !m_note.isEmpty();
    }

    inline void setTag(const QString &tag) noexcept
    {
        m_tag = tag;
    }

    inline QString tag() const noexcept
    {
        return m_tag;
    }


private:
    QPixmap m_pix;
    QString m_note = "";
    QString m_filename = "";
    QString m_tag = "";
    QSize m_pix_size = QSize(100, 100);
};

#endif
