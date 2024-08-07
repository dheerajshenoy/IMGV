#ifndef UTILS_HPP
#define UTILS_HPP

#include <qt6/QtCore/QString>
#include <qt6/QtCore/QFile>
#include <qt6/QtCore/QFileInfo>
#include <qt6/QtGui/QPixmap>
#include <qt6/QtCore/QMimeData>
#include <qt6/QtCore/QMimeDatabase>
#include <qt6/QtCore/QMimeType>
#include <webp/decode.h>

struct Dimension
{
    int width;
    int height;
};

class utils
{

public:
    utils(){}
    ~utils(){}

    static QString detectImageFormat(const QString &filePath);
    static QPixmap decodeWebPToPixmap(const QString &filePath);

};

#endif
