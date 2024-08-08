#ifndef THUMBNAIL_HPP
#define THUMBNAIL_HPP

#include <qt6/QtWidgets/QWidget>
#include <qt6/QtWidgets/QVBoxLayout>
#include <qt6/QtWidgets/QLabel>
#include <qt6/QtGui/QPixmap>
#include <qt6/QtCore/QFileInfo>
#include "utils.hpp"

class Thumbnail
{
public:
    Thumbnail();
    Thumbnail(const QString &filename) noexcept;
    ~Thumbnail() {}

    void setFilename(const QString &fileName) noexcept;
    void setNote(const QString &note) noexcept;
    QString note() noexcept;
    QString filename() noexcept;
    QPixmap pixmap() noexcept;

private:
    QVBoxLayout *m_layout;
    QLabel *m_imageLabel;
    QLabel *m_textLabel;
    QPixmap m_pix;
    QString m_note;
    QString m_filename;
};

#endif
