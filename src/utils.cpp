#include "utils.hpp"

QString utils::detectImageFormat(const QString &filePath) noexcept
{
    QMimeDatabase mimeDatabase;
    QFileInfo fileInfo(filePath);
    QMimeType mimeType = mimeDatabase.mimeTypeForFile(fileInfo);

    QString mimeName = mimeType.name();
    if (mimeName.startsWith("image/")) {
        // Extract the format from the MIME type
        QString format = mimeName.section('/', 1, 1);
        return format.toUpper();
    }

    return QString();
}

QPixmap utils::decodeWebPToPixmap(const QString &filePath) noexcept
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return QPixmap();
    }

    QByteArray webpData = file.readAll();
    file.close();

    int width = 0, height = 0;
    if (!WebPGetInfo(reinterpret_cast<const uint8_t *>(webpData.constData()), webpData.size(), &width, &height)) {
        return QPixmap();
    }

    uint8_t *decodedData = WebPDecodeRGBA(reinterpret_cast<const uint8_t *>(webpData.constData()), webpData.size(), &width, &height);
    if (!decodedData) {
        return QPixmap();
    }

    QImage image(decodedData, width, height, QImage::Format_RGBA8888);
    QPixmap pixmap = QPixmap::fromImage(image.copy());  // Copy to detach from raw data buffer
    WebPFree(decodedData);  // Free the decoded data buffer
    return pixmap;
}

QImage utils::decodeWebPToImage(const QString &filePath) noexcept
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return QImage();
    }

    QByteArray webpData = file.readAll();
    file.close();

    int width = 0, height = 0;
    if (!WebPGetInfo(reinterpret_cast<const uint8_t *>(webpData.constData()), webpData.size(), &width, &height)) {
        return QImage();
    }

    uint8_t *decodedData = WebPDecodeRGBA(reinterpret_cast<const uint8_t *>(webpData.constData()), webpData.size(), &width, &height);
    if (!decodedData) {
        return QImage();
    }

    QImage image(decodedData, width, height, QImage::Format_RGBA8888);
    WebPFree(decodedData);  // Free the decoded data buffer
    return image;
}

QPixmap utils::decodeHeicToPixmap(const QString &filePath) noexcept
{
    Magick::InitializeMagick(nullptr);
    Magick::Image image;
    image.read(filePath.toStdString());

    if (!image.isValid())
        return QPixmap();

    int width = image.columns(), height = image.rows();

    // Ensure the image is in RGB format for compatibility with QImage
    Magick::Blob blob;
    image.write(&blob, "RGBA");  // Convert to RGBA

    // Create a QImage using the raw data from the Magick::Image
    const uchar *data = reinterpret_cast<const uchar *>(blob.data());
    QImage qimage(data, width, height, QImage::Format_RGBA8888);

    return QPixmap::fromImage(qimage.copy());
}

QPixmap utils::decodeToPixmap(const QString &filePath) noexcept
{
    Magick::InitializeMagick(nullptr);
    Magick::Image image;
    image.read(filePath.toStdString());

    if (!image.isValid())
        return QPixmap();

    int width = image.columns(), height = image.rows();

    // Ensure the image is in RGB format for compatibility with QImage
    Magick::Blob blob;
    image.write(&blob, "RGBA");  // Convert to RGBA

    // Create a QImage using the raw data from the Magick::Image
    const uchar *data = reinterpret_cast<const uchar *>(blob.data());
    QImage qimage(data, width, height, QImage::Format_RGBA8888);

    return QPixmap::fromImage(qimage.copy());
}

QString utils::imageFormatToString(QImage::Format&& format) noexcept
{
    switch (format) {
        case QImage::Format_Invalid: return "Invalid";
        case QImage::Format_Mono: return "Mono";
        case QImage::Format_MonoLSB: return "MonoLSB";
        case QImage::Format_Indexed8: return "Indexed8";
        case QImage::Format_RGB32: return "RGB32";
        case QImage::Format_ARGB32: return "ARGB32";
        case QImage::Format_ARGB32_Premultiplied: return "ARGB32_Premultiplied";
        case QImage::Format_RGB16: return "RGB16";
        case QImage::Format_ARGB8565_Premultiplied: return "ARGB8565_Premultiplied";
        case QImage::Format_RGB666: return "RGB666";
        case QImage::Format_ARGB6666_Premultiplied: return "ARGB6666_Premultiplied";
        case QImage::Format_RGB555: return "RGB555";
        case QImage::Format_ARGB8555_Premultiplied: return "ARGB8555_Premultiplied";
        case QImage::Format_RGB888: return "RGB888";
        case QImage::Format_RGB444: return "RGB444";
        case QImage::Format_ARGB4444_Premultiplied: return "ARGB4444_Premultiplied";
        case QImage::Format_RGBX8888: return "RGBX8888";
        case QImage::Format_RGBA8888: return "RGBA8888";
        case QImage::Format_RGBA8888_Premultiplied: return "RGBA8888_Premultiplied";
        case QImage::Format_BGR30: return "BGR30";
        case QImage::Format_A2BGR30_Premultiplied: return "A2BGR30_Premultiplied";
        case QImage::Format_RGB30: return "RGB30";
        case QImage::Format_A2RGB30_Premultiplied: return "A2RGB30_Premultiplied";
        case QImage::Format_Alpha8: return "Alpha8";
        case QImage::Format_Grayscale8: return "Grayscale8";
        case QImage::Format_RGBX64: return "RGBX64";
        case QImage::Format_RGBA64: return "RGBA64";
        case QImage::Format_RGBA64_Premultiplied: return "RGBA64_Premultiplied";
        case QImage::Format_BGR888: return "BGR888";
        default: return "Unknown";
    }
}

QStringList utils::getImagesFromSessionFile(const QString &sessionfilepath) noexcept
{

    using namespace rapidjson;
    std::ifstream ifs(sessionfilepath.toStdString());

    if (!ifs.is_open()) {
        return {};
    }

    IStreamWrapper isw(ifs);

    Document doc;
    doc.ParseStream(isw);

    if (doc.HasParseError())
    {
        return {};
    }

    QStringList imgfiles;
    if (doc.HasMember("files") && doc["files"].IsArray())
    {
        const Value& files_arr = doc["files"];
        for(SizeType i=0; i < files_arr.Size(); i++)
        {
            if (files_arr[i].IsObject())
            {
                auto file = files_arr[i].GetObject();
                if (file.HasMember("path") && file["path"].IsString() && file.HasMember("note") && file["note"].IsString())
                {
                    imgfiles.emplaceBack(file["path"].GetString());
                }
            }
        }
    }
    ifs.close();
    return imgfiles;
}

Custom utils::getInfoFromSessionFile(const QString &sessionfilepath) noexcept
{
    using namespace rapidjson;
    std::ifstream ifs(sessionfilepath.toStdString());

    if (!ifs.is_open()) {
        return {};
    }

    IStreamWrapper isw(ifs);

    Document doc;
    doc.ParseStream(isw);

    if (doc.HasParseError())
    {
        return {};
    }

    QStringList imgfiles;
    if (doc.HasMember("files") && doc["files"].IsArray())
    {
        const Value& files_arr = doc["files"];
        for(SizeType i=0; i < files_arr.Size(); i++)
        {
            if (files_arr[i].IsObject())
            {
                auto file = files_arr[i].GetObject();
                if (file.HasMember("path") && file["path"].IsString() && file.HasMember("note") && file["note"].IsString())
                {
                    imgfiles.emplaceBack(file["path"].GetString());
                }
            }
        }
    }

    QStringList tags;
    if (doc.HasMember("tags") && doc["tags"].IsArray())
    {
        const Value& tags_arr = doc["tags"];
        for(SizeType i=0; i < tags_arr.Size(); i++)
            tags.emplaceBack(QString::fromStdString(tags_arr[i].GetString()));
    }

    QString date;
    if (doc.HasMember("date") && doc["date"].IsString())
        date = doc["date"].GetString();
    else
        date = "N/A";
    ifs.close();

    return Custom{ imgfiles, date, tags };

}
