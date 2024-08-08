#include "ThumbnailView.hpp"

ThumbnailView::ThumbnailView(QWidget *parent)
    : QListView(parent)
{
    setWindowIcon(QIcon(":/icons/imgv.png"));
    setViewMode(QListView::IconMode);
    setIconSize(QSize(100, 100));  // Set the size for thumbnails
    setResizeMode(QListView::Adjust);
    setSpacing(20);  // Spacing between thumbnails
    setTextElideMode(Qt::TextElideMode::ElideRight);
    setWordWrap(true);
    setSelectionMode(SelectionMode::ExtendedSelection);
    /*this->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);*/
    /*setUniformItemSizes(true);*/

    this->setContextMenuPolicy(Qt::CustomContextMenu);
    this->setModel(m_model);

    connect(this, &ThumbnailView::customContextMenuRequested, this, &ThumbnailView::showContextMenu);

    m_contextMenu->addAction(m_action__remove);
    m_contextMenu->addAction(m_action__hide);
    m_contextMenu->addAction(m_action__show_in_explorer);
    m_contextMenu->addAction(m_action__select_all);
    m_contextMenu->addAction(m_action__image_properties);

    connect(m_action__select_all, &QAction::triggered, this, [&]() {
        this->selectAll();
    });

    connect(m_action__image_properties, &QAction::triggered, this, &ThumbnailView::showProperties);

    m_partialContextMenu->addAction(m_action__select_all);

    connect(m_action__remove, &QAction::triggered, this, &ThumbnailView::removeThumbnails);
    connect(m_action__hide, &QAction::triggered, this, &ThumbnailView::hideThumbnails);
    connect(m_action__show_in_explorer, &QAction::triggered, this, &ThumbnailView::showFilesInExplorer);

    m_filter_proxy->setSourceModel(m_model);
}

void ThumbnailView::loadFile(const QString &file) noexcept
{
    Thumbnail thumb(file);
    m_model->addThumbnail(thumb);
}

ThumbnailModel* ThumbnailView::model() noexcept
{
    return m_model;
}

void ThumbnailView::createThumbnails(const QStringList &fileNames) noexcept
{
    for (const QString &fileName : fileNames) {
        Thumbnail thumb(fileName);
        m_model->addThumbnail(thumb);
    }

    if (m_model->rowCount() >= 0)
        setCurrentIndex(m_model->index(0));
}

void ThumbnailView::createThumbnails(const QList<Thumbnail> &thumbnails) noexcept
{
    for (const Thumbnail &thumb : thumbnails) {
        m_model->addThumbnail(thumb);
    }

    if (m_model->rowCount() >= 0)
        setCurrentIndex(m_model->index(0));
}

void ThumbnailView::dragEnterEvent(QDragEnterEvent *e)
{
    const QMimeData *mimedata = e->mimeData();
    if (mimedata->hasUrls())
        e->acceptProposedAction();
}

void ThumbnailView::dragLeaveEvent(QDragLeaveEvent *e)
{
    e->accept();
}

void ThumbnailView::dragMoveEvent(QDragMoveEvent *e)
{
    e->accept();
    e->acceptProposedAction();
}

void ThumbnailView::dropEvent(QDropEvent *e)
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
                m_model->addThumbnail(filepath);
            }
        }
    }
    e->acceptProposedAction();
}

void ThumbnailView::showContextMenu(const QPoint &pos) noexcept
{
    if (this->selectedIndexes().size() > 0 || this->indexAt(pos).isValid())
        m_contextMenu->exec(mapToGlobal(pos));
    else
        m_partialContextMenu->exec(mapToGlobal(pos));

}

void ThumbnailView::removeThumbnails()
{
    auto selections = this->selectedIndexes();
    for(const auto index : selections)
    {
        m_model->removeAt(index.row());
    }
}

void ThumbnailView::hideThumbnails()
{
    auto selections = this->selectedIndexes();
    for(const auto &s : selections)
        this->setRowHidden(s.row(), true);
}

void ThumbnailView::showFilesInExplorer()
{
    auto selection = this->selectedIndexes()[0];
    auto filepath = QFileInfo(selection.data(Qt::UserRole).toString()).path();
    QDesktopServices::openUrl(QUrl::fromLocalFile(filepath));
}

void ThumbnailView::gotoNext() noexcept
{
    if (currentIndex().row() < m_model->rowCount() - 1)
    {
        this->setCurrentIndex(currentIndex().siblingAtRow(currentIndex().row() + 1));
        emit fileChangeRequested(currentIndex().data(Qt::UserRole).toString());
    }
}

void ThumbnailView::gotoPrev() noexcept
{
    if (currentIndex().row() > 0)
    {
        this->setCurrentIndex(currentIndex().siblingAtRow(currentIndex().row() - 1));
        emit fileChangeRequested(currentIndex().data(Qt::UserRole).toString());
    }
}

void ThumbnailView::showProperties()
{
    QString filepath = this->selectedIndexes()[0].data(Qt::UserRole).toString();
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

void ThumbnailView::hideEvent(QHideEvent *e) noexcept
{
    emit visibilityChanged(isVisible());
}

void ThumbnailView::showEvent(QShowEvent *e) noexcept
{
    emit visibilityChanged(isVisible());
}

void ThumbnailView::addThumbnail(const QString &filename) noexcept
{
    m_model->addThumbnail(filename);
}

int ThumbnailView::count() noexcept
{
    return m_model->rowCount();
}

QString ThumbnailView::item(int _index, int role) noexcept
{
    auto index = this->currentIndex().siblingAtRow(_index);
    return m_model->data(index, role).toString();
}

void ThumbnailView::search(QString text) noexcept
{
    qDebug() << text;
    m_filter_proxy->setFilterText(text);
}

void ThumbnailView::searchMode(bool state) noexcept
{
    if (state)
        this->setModel(m_filter_proxy);
    else
        this->setModel(m_model);
}

Thumbnail ThumbnailView::currentThumbnail() noexcept
{
    return m_model->getThumbnail(currentIndex().row());
}

QString ThumbnailView::getFile(const int index) noexcept
{
    return m_model->index(index).data(Qt::UserRole).toString();
}
