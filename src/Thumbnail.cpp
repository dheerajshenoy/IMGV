#include "Thumbnail.hpp"

Thumbnail::Thumbnail()
{}

Thumbnail::Thumbnail(const QString &fileName) noexcept
    : m_filename(fileName)
{
    m_pix = utils::decodeToPixmap(fileName);
    if (!m_pix.isNull())
        m_pix = m_pix.scaled(m_pix_size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

Thumbnail::Thumbnail(const QString &fileName, const QPixmap &pix) noexcept
    : m_filename(fileName)
{
    if (!pix.isNull())
        m_pix = pix.scaled(m_pix_size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

Thumbnail::Thumbnail(const QString &fileName, const QString &note) noexcept
    : m_filename(fileName), m_note(note)
{
    m_pix = utils::decodeToPixmap(fileName);
    if (!m_pix.isNull())
        m_pix = m_pix.scaled(m_pix_size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

Thumbnail::Thumbnail(const QString &fileName, const QString &note, const QString &tag) noexcept
    : m_filename(fileName), m_note(note), m_tag(tag)
{
    m_pix = utils::decodeToPixmap(fileName);
    if (!m_pix.isNull())
        m_pix = m_pix.scaled(m_pix_size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

void Thumbnail::setThumbnailSize(const QSize& size) noexcept
{
    m_pix_size = size;
}
