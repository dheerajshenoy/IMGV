#ifndef UTILS_HPP
#define UTILS_HPP

#include <QString>
#include <QFile>
#include <QFileInfo>
#include <QStringList>
#include <QPixmap>
#include <QMimeData>
#include <QMimeDatabase>
#include <QMimeType>
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

// Utility struct for getting info from session files
struct Custom
{
    QStringList files;
    QString date;
    QStringList tags;
};

class utils
{

public:
    utils(){}
    ~utils(){}

    static QString detectImageFormat(const QString &filePath) noexcept;
    static QPixmap decodeWebPToPixmap(const QString &filePath) noexcept;
    static QImage decodeWebPToImage(const QString &filePath) noexcept;
    static QString imageFormatToString(QImage::Format&& format) noexcept;
    static QStringList getImagesFromSessionFile(const QString &session_file) noexcept;
    static Custom getInfoFromSessionFile(const QString &session_file) noexcept;
};

#endif
