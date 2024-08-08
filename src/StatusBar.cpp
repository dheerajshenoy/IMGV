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

    noteModifiedLabel->setVisible(false);

    filePathLabel->setToolTip("File path of the current Image");
    hasNoteLabel->setToolTip("This file has a note associated with it. Press the note key to open");
    noteModifiedLabel->setToolTip("Note has unsaved changes. Switching to other image will delete the unsaved changes");
    sessionLabel->setToolTip("Current session");

    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);


    setLayout(layout);
}

void StatusBar::defaultLayout() noexcept
{
    layout->setSpacing(10);
    layout->addWidget(msgLabel);
    layout->addWidget(filePathLabel);
    layout->addStretch(1);
    layout->addWidget(hasNoteLabel);
    layout->addWidget(noteModifiedLabel);
    layout->addWidget(fileSizeLabel);
    layout->addWidget(imageDimensionsLabel);
    layout->addWidget(sessionLabel);
}

void StatusBar::addWidget(const QString &name) noexcept
{
    if (name == "message")
        layout->addWidget(msgLabel);

    if (name == "path")
        layout->addWidget(filePathLabel);

    if (name == "note-indicator")
        layout->addWidget(hasNoteLabel);

    if (name == "note-modified-indicator")
        layout->addWidget(noteModifiedLabel);

    if (name == "size")
        layout->addWidget(fileSizeLabel);

    if (name == "dimension")
        layout->addWidget(imageDimensionsLabel);

    if (name == "stretch")
        layout->addStretch(1);

    if (name == "session")
        layout->addWidget(sessionLabel);
}

void StatusBar::setSessionName(QString sess) noexcept
{
    sessionLabel->setText(sess);
}

void StatusBar::updateFileInfo(const QString &filePath)
{
    QFileInfo fileInfo(filePath);

    if (fileInfo.exists()) {
        // Update file path
        filePathLabel->setText(filePath);

        // Update file size
        qint64 fileSize = fileInfo.size();
        QLocale locale;
        fileSizeLabel->setText(QString("Size: %1").arg(locale.formattedDataSize(fileSize)));

    } else {
        filePathLabel->setText("N/A");
        fileSizeLabel->setText("Size: N/A");
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

void StatusBar::setNoteModified(bool state) noexcept
{
    if (state)
    {
        noteModifiedLabel->setText(m_note_modified_indicator);
        noteModifiedLabel->setVisible(true);
    }
    else
        noteModifiedLabel->setVisible(false);

}

void StatusBar::modificationLabelVisiblity(bool state) noexcept
{
    /*if (state)*/
    /*    this->noteModifiedLabel->setVisible(true);*/
    /*else*/
    /*if (!state)*/
    /*    this->noteModifiedLabel->setVisible(false);*/
}

void StatusBar::setHasNote(bool state) noexcept
{
    if (state)
    {
        hasNoteLabel->setText(m_note_indicator);
        hasNoteLabel->setVisible(true);
    }
    else
        hasNoteLabel->setHidden(true);
}

void StatusBar::setSpacing(const int spacing) noexcept
{
    layout->setSpacing(spacing);
}

void StatusBar::setNoteIndicator(const QString &indicator) noexcept
{
    m_note_indicator = indicator;
}

void StatusBar::setNoteModifiedIndicator(const QString &indicator) noexcept
{
    m_note_modified_indicator = indicator;
}
