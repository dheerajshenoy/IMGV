#include "Thumbnail.hpp"

Thumbnail::Thumbnail(QWidget *parent)
    : QWidget(parent)
{
    m_layout = new QVBoxLayout(this);

    m_imageLabel = new QLabel(this);
    m_imageLabel->setAlignment(Qt::AlignCenter);

    m_textLabel = new QLabel(this);
    m_textLabel->setAlignment(Qt::AlignCenter);

    m_layout->addWidget(m_imageLabel);
    m_layout->addWidget(m_textLabel);

    setLayout(m_layout);

}

void Thumbnail::setFilename(const QString &fileName)
{
    QFileInfo fileInfo(fileName);
    QPixmap pixmap(fileName);
    
    if (!pixmap.isNull()) {
        m_imageLabel->setPixmap(pixmap.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        m_imageLabel->clear();
    }

    m_textLabel->setText(fileInfo.fileName());
}
