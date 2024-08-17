#include "ImagePropertiesDialog.hpp"

ImagePropertiesDialog::ImagePropertiesDialog(const QString& _filename, QWidget *parent)
    : QDialog(parent)
{

    QString _format = utils::detectImageFormat(_filename);
    QImage img;
    if (_format == "WEBP")
        img = utils::decodeWebPToImage(_filename);
    else
        img = QImage(_filename);

    QLocale locale;
    QFormLayout *layout = new QFormLayout(this);
    QLabel *filename = new QLabel(QFileInfo(_filename).baseName());
    QLabel *filepath = new QLabel(_filename);
    QLabel *label_filesize = new QLabel("Filesize:");
    QLabel *width = new QLabel(QString::number(img.width()));
    QLabel *height = new QLabel(QString::number(img.height()));
    QLabel *filesize = new QLabel(locale.formattedDataSize(img.sizeInBytes()));
    QLabel *colorspace = new QLabel(utils::imageFormatToString(img.format()));
    QLabel *format = new QLabel(_format);
    QLabel *aspect_ratio = new QLabel(QString::number(img.width() / img.height()));
    QLabel *bits_per_pixel = new QLabel(QString::number(img.depth()));
    QLabel *has_alpha_channel = new QLabel(QString::number(img.hasAlphaChannel()));
    QLabel *is_grayscale = new QLabel(QString::number(img.isGrayscale()));
    QLabel *dpi_x = new QLabel(QString::number(int(img.dotsPerMeterX() * 0.0254)));
    QLabel *dpi_y = new QLabel(QString::number(int(img.dotsPerMeterY() * 0.0254)));

    layout->addRow("File Name: ", filename);
    layout->addRow("Path: ", filepath);
    layout->addRow("Size: ", filesize);
    layout->addRow("Width: ", width);
    layout->addRow("Height: ", height);
    layout->addRow("Colorspace: ", colorspace);
    layout->addRow("Format: ", format);
    layout->addRow("Aspect Ratio: ", aspect_ratio);
    layout->addRow("Bits per pixel: ", bits_per_pixel);
    layout->addRow("Has alpha channel: ", has_alpha_channel);
    layout->addRow("Is grayscale: ", is_grayscale);
    layout->addRow("DPI x: ", dpi_x);
    layout->addRow("DPI y: ", dpi_y);

    if (_format == "JPEG")
    {
            QPushButton *exif_btn = new QPushButton("EXIF Metadata");
            connect(exif_btn, &QPushButton::clicked, this, [&, _filename]() { showEXIFProperties(_filename); });
            layout->addWidget(exif_btn);
    }

    QPushButton *ok_btn = new QPushButton("Ok");
    layout->addWidget(ok_btn);
    connect(ok_btn, &QPushButton::clicked, this, &QDialog::done);
}

void ImagePropertiesDialog::showEXIFProperties(const QString& filename) noexcept
{
    using namespace easyexif;
    EXIFInfo exifInfo;
    QFile file(filename);
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
