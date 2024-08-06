#include "StatusBar.hpp"

StatusBar::StatusBar(QWidget *parent)
    : QWidget(parent)
{
    layout = new QHBoxLayout(this);

    filePathLabel = new QLabel(this);
    fileSizeLabel = new QLabel(this);
    imageDimensionsLabel = new QLabel(this);

    layout->addWidget(filePathLabel);
    layout->addStretch(1);
    layout->addWidget(fileSizeLabel);
    layout->addSpacing(10);
    layout->addWidget(imageDimensionsLabel);

    this->setContentsMargins(0, 0, 0, 0);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);


    setLayout(layout);
}

void StatusBar::updateFileInfo(const QString &filePath)
{
    QFileInfo fileInfo(filePath);

    if (fileInfo.exists()) {
        // Update file path
        filePathLabel->setText(QString("File: %1").arg(filePath));

        // Update file size
        qint64 fileSize = fileInfo.size();
        QLocale locale;
        fileSizeLabel->setText(QString("Size: %1").arg(locale.formattedDataSize(fileSize)));

        // Update image dimensions
        QPixmap pixmap(filePath);
        if (!pixmap.isNull()) {
            imageDimensionsLabel->setText(QString("Dimensions: (%1, %2)").arg(pixmap.width()).arg(pixmap.height()));
        } else {
            imageDimensionsLabel->setText("Dimensions: N/A");
        }
    } else {
        filePathLabel->setText("File: N/A");
        fileSizeLabel->setText("Size: N/A");
        imageDimensionsLabel->setText("Dimensions: N/A");
    }
}
