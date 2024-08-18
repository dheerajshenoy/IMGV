#include "ImagePropertiesDialog.hpp"
#include "rapidjson/document.h"
#include <qtablewidget.h>

ImagePropertiesDialog::ImagePropertiesDialog(const QString& _filename, QWidget *parent)
    : QDialog(parent)
{

    QString _format = utils::detectImageFormat(_filename);
    QImage img;
    img = utils::decodeToPixmap(_filename).toImage();

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
    std::vector<std::string> exifAttributes = {
        "EXIF:Make",
        "EXIF:Model",
        "EXIF:ExposureTime",
        "EXIF:FNumber",
        "EXIF:ISOSpeedRatings",
        "EXIF:DateTime",
        "EXIF:ShutterSpeedValue",
        "EXIF:ApertureValue",
        "EXIF:ExposureBiasValue",
        "EXIF:MeteringMode",
        "EXIF:Flash",
        "EXIF:FocalLength",
        "EXIF:WhiteBalance",
        "EXIF:GPSLatitude",
        "EXIF:GPSLongitude",
        "EXIF:GPSAltitude",
        "EXIF:Software",
        "EXIF:Copyright",
        "EXIF:Artist",
        "EXIF:Orientation",
        "EXIF:ImageWidth",
        "EXIF:ImageHeight",
        "EXIF:ResolutionUnit",
        "EXIF:ExifImageWidth",
        "EXIF:ExifImageHeight",
        "EXIF:ExposureProgram",
        "EXIF:DigitalZoomRatio",
        "EXIF:Contrast",
        "EXIF:Saturation",
        "EXIF:Sharpness",
        "EXIF:SubjectDistance",
        "EXIF:SceneCaptureType",
        "EXIF:GainControl",
        "EXIF:MaxApertureValue",
        "EXIF:LightSource"
    };

    constexpr unsigned int size = 34;

    Magick::InitializeMagick(nullptr);

    // Load the image
    Magick::Image image;


    image.read(filename.toStdString());

    QDialog *d = new QDialog(this);
    QFormLayout *layout = new QFormLayout(d);
    QTableWidget *exifTable = new QTableWidget(size, 2, this);
    QPushButton *export_to_json_btn = new QPushButton("Export to JSON");
    layout->addWidget(exifTable);
    layout->addWidget(export_to_json_btn);

    exifTable->setHorizontalHeaderLabels({"EXIF Attribute", "Value"});
    exifTable->horizontalHeader()->setStretchLastSection(true);
    exifTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Stretch);
    exifTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    exifTable->setSelectionBehavior(QAbstractItemView::SelectRows);

    for(int i=0; i < size; i++)
    {
        auto attribute = exifAttributes.at(i);
        QTableWidgetItem *attribute_name_item = new QTableWidgetItem(QString::fromStdString(attribute));
        auto value = QString::fromStdString(image.attribute(attribute));
        QTableWidgetItem *attribute_value_item = new QTableWidgetItem(value);
        exifTable->setItem(i, 0, attribute_name_item);
        exifTable->setItem(i, 1, attribute_value_item);
    }

    QCheckBox *m_toggle_no_data_rows_cb = new QCheckBox("Hide rows with no values");
    layout->addWidget(m_toggle_no_data_rows_cb);

    connect(m_toggle_no_data_rows_cb, &QCheckBox::toggled, [exifTable](const bool &state) {
        exifTable->setUpdatesEnabled(false);
        for (int row = 0; row < exifTable->rowCount(); row++)
            if (exifTable->item(row, 1)->text().isEmpty())
                exifTable->setRowHidden(row, state);
        exifTable->setUpdatesEnabled(true);
    });

    connect(export_to_json_btn, &QPushButton::clicked, this, [&, exifTable]() {
        QString filename = QFileDialog::getSaveFileName(this, "Save EXIF Metadata", "Select file", tr("Images (*.png *.jpg *.bmp *.gif *.svg *.webp *.heic *.heif)"));
        if (filename.isEmpty())
            return;

        // Initialize RapidJSON document
        rapidjson::Document document;
        document.SetObject();
        rapidjson::Document::AllocatorType& allocator = document.GetAllocator();

        // Create a JSON array to hold EXIF attributes and values
        rapidjson::Value exifArray(rapidjson::kArrayType);

        // Create a single JSON object to hold all EXIF key-value pairs
        rapidjson::Value exifObject(rapidjson::kObjectType);

        // Iterate over rows of the QTableWidget
        for (int i = 0; i < size; ++i)
        {
            QTableWidgetItem *attributeItem = exifTable->item(i, 0);
            QTableWidgetItem *valueItem = exifTable->item(i, 1);

            // Ensure items are valid
            if (attributeItem && valueItem)
            {
                QString attribute = attributeItem->text();
                QString value = valueItem->text();

                // Add the attribute and its value to the EXIF object
                exifObject.AddMember(
                    rapidjson::Value(attribute.toStdString().c_str(), allocator),
                    rapidjson::Value(value.toStdString().c_str(), allocator),
                    allocator
                );
            }
        }

        // Add the EXIF object to the array
        exifArray.PushBack(exifObject, allocator);

        // Add the EXIF array to the document with the key "exif"
        document.AddMember("exif", exifArray, allocator);

        QMessageBox mini_or_readable_msg_box;
        QAbstractButton *miniBtn = mini_or_readable_msg_box.addButton("Mini Verson", QMessageBox::YesRole);
        QAbstractButton *readableBtn = mini_or_readable_msg_box.addButton("Readable Version", QMessageBox::NoRole);

        mini_or_readable_msg_box.setText("Do you want the \"mini\" version of the JSON file or file which is pretty and readable ? If you don't know what \"mini\" files are, go with the pretty version.");
        mini_or_readable_msg_box.setWindowTitle("JSON write mode");
        mini_or_readable_msg_box.exec();

        // Convert JSON document to string
        rapidjson::StringBuffer buffer;

        if (mini_or_readable_msg_box.clickedButton() == miniBtn)
        {
            rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
            document.Accept(writer);
        }
        else if (mini_or_readable_msg_box.clickedButton() == readableBtn)
        {
            rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
            document.Accept(writer);
        }
        
        // Write JSON string to file
        QFile file(filename);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << QString::fromUtf8(buffer.GetString(), buffer.GetSize());
            file.close();
        } else {
            qWarning() << "Unable to open file for writing:" << filename;
        }
        });


    d->open();
}
