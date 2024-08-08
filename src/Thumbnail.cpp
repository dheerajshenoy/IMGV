#include "Thumbnail.hpp"

Thumbnail::Thumbnail()
{}

Thumbnail::Thumbnail(const QString &fileName) noexcept
{
    if (utils::detectImageFormat(fileName) != "WEBP")
    {
        QPixmap pixmap(fileName);
        if (pixmap.isNull()) return;
        m_pix = pixmap.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    else
        m_pix = utils::decodeWebPToPixmap(fileName).scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    m_filename = fileName;
}

QString Thumbnail::note() noexcept
{
    return m_note;
}

QPixmap Thumbnail::pixmap() noexcept
{
    return m_pix;
}

QString Thumbnail::filename() noexcept
{
    return m_filename;
}

void Thumbnail::setNote(const QString &note) noexcept
{
    if (note.isNull()) m_note = note;
}

void Thumbnail::setFilename(const QString &filename) noexcept
{
    m_filename = filename;
}
