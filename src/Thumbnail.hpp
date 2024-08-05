#ifndef THUMBNAIL_HPP
#define THUMBNAIL_HPP

#include <qt6/QtWidgets/QWidget>
#include <qt6/QtWidgets/QVBoxLayout>
#include <qt6/QtWidgets/QLabel>
#include <qt6/QtGui/QPixmap>
#include <qt6/QtCore/QFileInfo>

class Thumbnail : public QWidget
{
public:
    Thumbnail(QWidget *parent = nullptr);
    Thumbnail() {}

    void setFilename(const QString &fileName);

private:
    QVBoxLayout *m_layout;
    QLabel *m_imageLabel;
    QLabel *m_textLabel;
};

#endif
