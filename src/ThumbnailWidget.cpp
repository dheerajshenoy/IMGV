#include "ThumbnailWidget.hpp"
#include "qnamespace.h"

ThumbnailWidget::ThumbnailWidget(QWidget *parent)
    : QListWidget(parent)
{
    
    setViewMode(QListWidget::IconMode);
    setIconSize(QSize(100, 100));  // Set the size for thumbnails
    setResizeMode(QListWidget::Adjust);
    setSpacing(20);  // Spacing between thumbnails
    setTextElideMode(Qt::TextElideMode::ElideRight);
    setWordWrap(true);
    setSelectionMode(SelectionMode::ExtendedSelection);
    /*this->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);*/
    /*setUniformItemSizes(true);*/

    this->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(this, &ThumbnailWidget::customContextMenuRequested, this, &ThumbnailWidget::showContextMenu);

    m_contextMenu->addAction(m_action__remove);
    m_contextMenu->addAction(m_action__hide);
    m_contextMenu->addAction(m_action__show_in_explorer);
    m_contextMenu->addAction(m_action__select_all);
    m_contextMenu->addAction(m_action__image_properties);

    connect(m_action__select_all, &QAction::triggered, this, [&]() {
        this->selectAll();
    });

    connect(m_action__image_properties, &QAction::triggered, this, &ThumbnailWidget::showProperties);

    m_partialContextMenu->addAction(m_action__select_all);

    connect(m_action__remove, &QAction::triggered, this, &ThumbnailWidget::removeThumbnails);
    connect(m_action__hide, &QAction::triggered, this, &ThumbnailWidget::hideThumbnails);
    connect(m_action__show_in_explorer, &QAction::triggered, this, &ThumbnailWidget::showFilesInExplorer);
}

void ThumbnailWidget::createThumbnails(const QStringList &fileNames)
{
    for (const QString &fileName : fileNames) {
        QPixmap thumbnail = createThumbnail(fileName, QSize(100, 100));
        if (!thumbnail.isNull()) {
            QListWidgetItem *item = new QListWidgetItem(QIcon(thumbnail), QFileInfo(fileName).fileName());
            item->setData(Qt::UserRole, fileName); // Store full file path in item
            addItem(item);
        } else {
            qWarning("Failed to create thumbnail for %s", qPrintable(fileName));
        }
    }
    setCurrentItem(this->item(this->count() - 1));
}

QPixmap ThumbnailWidget::createThumbnail(const QString &fileName, const QSize &size)
{
    if (utils::detectImageFormat(fileName) != "WEBP")
    {
        QPixmap pixmap = QPixmap(fileName);

        if (pixmap.isNull()) return QPixmap();

        return pixmap.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    else
        return utils::decodeWebPToPixmap(fileName).scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}


void ThumbnailWidget::dragEnterEvent(QDragEnterEvent *e)
{
    const QMimeData *mimedata = e->mimeData();

    if (mimedata->hasUrls())
        e->acceptProposedAction();

}

void ThumbnailWidget::dragLeaveEvent(QDragLeaveEvent *e)
{
    e->accept();
}

void ThumbnailWidget::dragMoveEvent(QDragMoveEvent *e)
{
    e->accept();
    e->acceptProposedAction();
}

void ThumbnailWidget::dropEvent(QDropEvent *e)
{
    /*if (e->source() == this) return;*/
    if (e->mimeData()->hasUrls())
    {
        QList<QUrl> urllist = e->mimeData()->urls();
        foreach(QUrl url, urllist)
        {
            if (url.isLocalFile())
            {
                QString filepath = url.toLocalFile();
                addThumbnail(filepath);
            }
        }
    }
    e->acceptProposedAction();
}

void ThumbnailWidget::addThumbnail(const QString &fileName)
{
    QPixmap thumbnail = createThumbnail(fileName, QSize(100, 100));
    if (!thumbnail.isNull()) {
        QListWidgetItem *item = new QListWidgetItem(QIcon(thumbnail), QFileInfo(fileName).fileName());
        item->setData(Qt::UserRole, fileName); // Store full file path in item
        addItem(item);
        setCurrentItem(item);
    } else {
        qWarning("Failed to create thumbnail for %s", qPrintable(fileName));
    }
}

void ThumbnailWidget::showContextMenu(const QPoint &pos) noexcept
{
    if (this->selectedItems().size() > 0 || this->itemAt(pos))
        m_contextMenu->exec(mapToGlobal(pos));
    else
        m_partialContextMenu->exec(mapToGlobal(pos));
}

void ThumbnailWidget::removeThumbnails()
{
    auto selections = this->selectedItems();
    for(const QListWidgetItem *s : selections)
    {
        delete this->takeItem(this->row(s));
    }
}

void ThumbnailWidget::hideThumbnails()
{

    auto selections = this->selectedItems();
    for(const auto &s : selections)
    {
        s->setHidden(true);
    }

}

void ThumbnailWidget::showFilesInExplorer()
{
    auto selection = this->selectedItems()[0];
    auto filepath = QFileInfo(selection->data(Qt::UserRole).toString()).path();
    QDesktopServices::openUrl(QUrl::fromLocalFile(filepath));
}

void ThumbnailWidget::gotoNext()
{
    if (currentRow() < count() - 1)
    {
        this->setCurrentRow(currentRow() + 1);
        emit fileChangeRequested(currentItem()->data(Qt::UserRole).toString());
    }
}

void ThumbnailWidget::gotoPrev()
{
    if (currentRow() > 0)
    {
        this->setCurrentRow(currentRow() - 1);
        emit fileChangeRequested(currentItem()->data(Qt::UserRole).toString());
    }
}

void ThumbnailWidget::showProperties()
{
    QString filepath = this->selectedItems()[0]->data(Qt::UserRole).toString();
    using namespace easyexif;
    auto format = utils::detectImageFormat(filepath);
    if ( format == "JPEG" )
    {
        EXIFInfo exifInfo;
        QFile file(filepath);
        file.open(QIODevice::ReadOnly);
        auto data = file.readAll();
        file.close();
        exifInfo.parseFrom(reinterpret_cast<const unsigned char*>(data.constData()), data.size());
        QString exifData = QString("Camera make       : %1\n"
                                   "Camera model      : %2\n"
                                   "Software          : %3\n"
                                   "Bits per sample   : %4\n"
                                   "Image width       : %5\n"
                                   "Image height      : %6\n"
                                   "Image description : %7\n"
                                   "Image orientation : %8\n"
                                   "Image copyright   : %9\n"
                                   "Image date/time   : %10\n"
                                   "Original date/time: %11\n"
                                   "Digitize date/time: %12\n"
                                   "Subsecond time    : %13\n"
                                   "Exposure time     : 1/%14 s\n"
                                   "F-stop            : f/%15\n"
                                   "ISO speed         : %16\n"
                                   "Subject distance  : %17 m\n"
                                   "Exposure bias     : %18 EV\n"
                                   "Flash used?       : %19\n"
                                   "Metering mode     : %20\n"
                                   "Lens focal length : %21 mm\n"
                                   "35mm focal length : %22 mm\n"
                                   "GPS Latitude      : %23 deg\n"
                                   "GPS Longitude     : %24 deg\n"
                                   "GPS Altitude      : %25 m")
            .arg(QString::fromStdString(exifInfo.Make))
            .arg(QString::fromStdString(exifInfo.Model))
            .arg(QString::fromStdString(exifInfo.Software))
            .arg(exifInfo.BitsPerSample)
            .arg(exifInfo.ImageWidth)
            .arg(exifInfo.ImageHeight)
            .arg(QString::fromStdString(exifInfo.ImageDescription))
            .arg(exifInfo.Orientation)
            .arg(QString::fromStdString(exifInfo.Copyright))
            .arg(QString::fromStdString(exifInfo.DateTime))
            .arg(QString::fromStdString(exifInfo.DateTimeOriginal))
            .arg(QString::fromStdString(exifInfo.DateTimeDigitized))
            .arg(QString::fromStdString(exifInfo.SubSecTimeOriginal))
            .arg(static_cast<unsigned>(1.0 / exifInfo.ExposureTime))
            .arg(exifInfo.FNumber, 0, 'f', 1)
            .arg(exifInfo.ISOSpeedRatings)
            .arg(exifInfo.SubjectDistance, 0, 'f', 2)
            .arg(exifInfo.ExposureBiasValue, 0, 'f', 2)
            .arg(exifInfo.Flash)
            .arg(exifInfo.MeteringMode)
            .arg(exifInfo.FocalLength, 0, 'f', 2)
            .arg(exifInfo.FocalLengthIn35mm)
            .arg(exifInfo.GeoLocation.Latitude, 0, 'f', 6)
            .arg(exifInfo.GeoLocation.Longitude, 0, 'f', 6)
            .arg(exifInfo.GeoLocation.Altitude, 0, 'f', 2);
        QMessageBox::information(this, "Metadata", exifData);
    }
    else QMessageBox::information(this, "Metadata Retreival Error", "Sorry. But this format doesn't support Image Metadata");
}

void ThumbnailWidget::hideEvent(QHideEvent *e) noexcept
{
    emit visibilityChanged(isVisible());
}

void ThumbnailWidget::showEvent(QShowEvent *e) noexcept
{
    emit visibilityChanged(isVisible());
}
