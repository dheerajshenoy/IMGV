#include "ThumbnailWidget.hpp"

ThumbnailWidget::ThumbnailWidget(QWidget *parent)
    : QListWidget(parent)
{
    
    setViewMode(QListWidget::IconMode);
    setIconSize(QSize(100, 100));  // Set the size for thumbnails
    setResizeMode(QListWidget::Adjust);
    setSpacing(20);  // Spacing between thumbnails
    setTextElideMode(Qt::TextElideMode::ElideRight);
    setWordWrap(true);
    /*setUniformItemSizes(true);*/
}

void ThumbnailWidget::createThumbnails(const QStringList &fileNames)
{
    for (const QString &fileName : fileNames) {
        QPixmap thumbnail = createThumbnail(fileName, QSize(100, 100));
        if (!thumbnail.isNull()) {
            QListWidgetItem *item = new QListWidgetItem(QIcon(thumbnail), QFileInfo(fileName).baseName());
            item->setData(Qt::UserRole, fileName); // Store full file path in item
            addItem(item);
        } else {
            qWarning("Failed to create thumbnail for %s", qPrintable(fileName));
        }
    }
}

QPixmap ThumbnailWidget::createThumbnail(const QString &fileName, const QSize &size)
{
    QImage image(fileName);
    if (image.isNull()) {
        return QPixmap();  // Return an empty pixmap if the image fails to load
    }

    QPixmap pixmap = QPixmap::fromImage(image);
    return pixmap.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}
