#include "StatusBar.hpp"

StatusBar::StatusBar(QWidget *parent)
    : QWidget(parent)
{
    layout = new QHBoxLayout(this);

    msgLabel = new QLabel(this);
    filePathLabel = new QLabel(this);
    fileSizeLabel = new QLabel(this);
    imageDimensionsLabel = new QLabel(this);

    msgLabel->setHidden(true);

    layout->addWidget(msgLabel);
    layout->addWidget(filePathLabel);
    layout->addStretch(1);
    layout->addWidget(fileSizeLabel);
    layout->addSpacing(10);
    layout->addWidget(imageDimensionsLabel);
    layout->addSpacing(10);
    layout->addWidget(sessionLabel);

    this->setContentsMargins(0, 0, 0, 0);
    layout->setContentsMargins(0, 0, 0, 0);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);


    setLayout(layout);
}

void StatusBar::setSessionName(QString sess) noexcept
{
    sessionLabel->setText("Session: " + sess);
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

    } else {
        filePathLabel->setText("File: N/A");
        fileSizeLabel->setText("Size: N/A");
        imageDimensionsLabel->setText("Dimensions: N/A");
    }
}


void StatusBar::hideEvent(QHideEvent *e) noexcept
{
    emit visibilityChanged(isVisible());
}

void StatusBar::showEvent(QShowEvent *e) noexcept
{
    emit visibilityChanged(isVisible());
}

void StatusBar::clearTexts() noexcept
{
    this->sessionLabel->clear();
    this->filePathLabel->clear();
    this->fileSizeLabel->clear();
    this->imageDimensionsLabel->clear();
}

void StatusBar::setMsg(QString msg, int sec) noexcept
{
    msgLabel->setText(msg);
    filePathLabel->setHidden(true);

    QTimer::singleShot(sec * 1000, [&]() {
        filePathLabel->setHidden(false);
    });
}

void StatusBar::setImgDimension(const int w, const int h) noexcept
{
    imageDimensionsLabel->setText(QString("(%1,%2)").arg(w).arg(h));
}
