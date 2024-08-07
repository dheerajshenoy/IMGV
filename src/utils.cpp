#include "utils.hpp"

QString utils::detectImageFormat(const QString &filePath) {
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

QPixmap utils::decodeWebPToPixmap(const QString &filePath) {
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
