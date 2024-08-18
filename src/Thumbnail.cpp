#include "Thumbnail.hpp"

Thumbnail::Thumbnail()
{}

Thumbnail::Thumbnail(const QString &fileName) noexcept
{
    auto format = utils::detectImageFormat(fileName);

    if (format == "WEBP")
    {
        m_pix = utils::decodeWebPToPixmap(fileName).scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    else if (format == "HEIC" || format == "HEIF")
    {
        m_pix = utils::decodeHeicToPixmap(fileName).scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    else
    {
        QPixmap pixmap(fileName);
        if (pixmap.isNull()) return;
        m_pix = pixmap.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    m_filename = fileName;
    m_note = "";
}

Thumbnail::Thumbnail(const QString &fileName, const QPixmap &pix) noexcept
{
    if (pix.isNull()) return;
    m_pix = pix.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    m_filename = fileName;
    m_note = "";
}

Thumbnail::Thumbnail(const QString &fileName, const QString &note) noexcept
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
    m_note = note;
}

Thumbnail::Thumbnail(const QString &fileName, const QString &note, const QString &tag) noexcept
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
    m_note = note;
    m_tag = tag;
}

