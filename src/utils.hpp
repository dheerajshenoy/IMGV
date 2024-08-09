#ifndef UTILS_HPP
#define UTILS_HPP

#include <qt6/QtCore/QString>
#include <qt6/QtCore/QFile>
#include <qt6/QtCore/QFileInfo>
#include <qt6/QtCore/QStringList>
#include <qt6/QtGui/QPixmap>
#include <qt6/QtCore/QMimeData>
#include <qt6/QtCore/QMimeDatabase>
#include <qt6/QtCore/QMimeType>
#include <webp/decode.h>
#include <fstream>
#include "rapidjson/rapidjson.h"
#include "rapidjson/reader.h"
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/document.h"

struct Dimension
{
    int width;
    int height;
};

struct Custom
{
    QStringList files;
    QString date;
};

class utils
{

public:
    utils(){}
    ~utils(){}

    static QString detectImageFormat(const QString &filePath) noexcept;
    static QPixmap decodeWebPToPixmap(const QString &filePath) noexcept;
    static QImage decodeWebPToImage(const QString &filePath) noexcept;
    static QString imageFormatToString(const QImage::Format format) noexcept;
    static QStringList getImagesFromSessionFile(const QString &session_file) noexcept;
    static Custom getDateandImagesFromSessionFile(const QString &session_file) noexcept;
};

#endif
