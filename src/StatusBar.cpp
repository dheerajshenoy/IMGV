#include "StatusBar.hpp"

StatusBar::StatusBar(QWidget *parent)
    : QWidget(parent)
{
    layout = new QHBoxLayout();
    layout->setContentsMargins(2, 2, 2, 2);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    setLayout(layout);
}

void StatusBar::defaultLayout() noexcept
{
    msgLabel = new QLabel();
    hoverFilePathLabel = new HoverLabel();
    hasNoteLabel = new QLabel();
    noteModifiedLabel = new QLabel();
    zoomLabel = new QLabel();
    fileSizeLabel = new QLabel();
    imageDimensionsLabel = new QLabel();
    sessionLabel = new QLabel("No Session");

    msgLabel->setHidden(true);
    noteModifiedLabel->setVisible(false);
    hasNoteLabel->setToolTip("This file has a note associated with it. Press the note key to open");
    hoverFilePathLabel->setToolTip("File path of the current Image");
    sessionLabel->setToolTip("Current session");

    layout->setSpacing(10);
    layout->addWidget(msgLabel);
    layout->addWidget(hoverFilePathLabel);
    layout->addStretch(1);
    layout->addWidget(hasNoteLabel);
    layout->addWidget(noteModifiedLabel);
    layout->addWidget(zoomLabel);
    layout->addWidget(fileSizeLabel);
    layout->addWidget(imageDimensionsLabel);
    layout->addWidget(sessionLabel);
}

void StatusBar::addWidget(const QString &name) noexcept
{
    if (name == "message")
    {
        msgLabel = new QLabel();
        layout->addWidget(msgLabel);
        msgLabel->setHidden(true);
    }

    else if (name == "note-indicator")
    {
        hasNoteLabel = new QLabel();
        layout->addWidget(hasNoteLabel);
        hasNoteLabel->setToolTip("This file has a note associated with it. Press the note key to open");
    }

    else if (name == "note-modified-indicator")
    {
        noteModifiedLabel = new QLabel();
        layout->addWidget(noteModifiedLabel);
        noteModifiedLabel->setVisible(false);
    }

    else if (name == "size")
    {
        fileSizeLabel = new QLabel();
        layout->addWidget(fileSizeLabel);
    }

    else if (name == "dimension")
    {
        imageDimensionsLabel = new QLabel();
        layout->addWidget(imageDimensionsLabel);
    }

    else if (name == "stretch")
        layout->addStretch(1);

    else if (name == "session")
    {
        sessionLabel = new QLabel("No Session");
        layout->addWidget(sessionLabel);
        sessionLabel->setToolTip("Current session");
    }

    else if (name == "zoom")
    {
        zoomLabel = new QLabel();
        layout->addWidget(zoomLabel);
    }

    else if (name == "path")
    {
        hoverFilePathLabel = new HoverLabel();
        layout->addWidget(hoverFilePathLabel);
        hoverFilePathLabel->setToolTip("File path of the current Image");
    }
}

void StatusBar::setSessionName(QString sess) noexcept
{
    if (sessionLabel)
        sessionLabel->setText(sess);
}

void StatusBar::updateFileInfo(const QString &filePath) noexcept
{
    QFileInfo fileInfo(filePath);

    if (fileInfo.exists()) {
        // Update file path
        /*filePathLabel->setText(filePath);*/

        if (hoverFilePathLabel)
        {
            hoverFilePathLabel->setFullText(filePath);
            hoverFilePathLabel->setShowText(fileInfo.fileName());
        }

        if (fileSizeLabel)
        {
            // Update file size
            QLocale locale;
            fileSizeLabel->setText(QString("Size: %1").arg(locale.formattedDataSize(fileInfo.size())));
        }

    } else {
        /*filePathLabel->setText("N/A");*/
        if (hoverFilePathLabel)
            hoverFilePathLabel->clear();
        if (fileSizeLabel)
            fileSizeLabel->setText("Size: N/A");
    }
}


void StatusBar::clearTexts() noexcept
{
    if (sessionLabel)
        sessionLabel->clear();

    if (hoverFilePathLabel)
        hoverFilePathLabel->clear();

    if (fileSizeLabel)
        fileSizeLabel->clear();

    if (imageDimensionsLabel)
        imageDimensionsLabel->clear();
}

void StatusBar::setMsg(QString msg, int sec) noexcept
{
    if (msgLabel)
    {
        msgLabel->setText(msg);
        msgLabel->setHidden(false);
        /*filePathLabel->setHidden(true);*/
        hoverFilePathLabel->setHidden(true);

        QTimer::singleShot(sec * 1000, [&]() {
            /*filePathLabel->setHidden(false);*/
            hoverFilePathLabel->setHidden(false);
            msgLabel->setHidden(true);
        });
    }
}

void StatusBar::setNoteModified(bool state) noexcept
{
    if (noteModifiedLabel)
    {
        if (state)
        {
            noteModifiedLabel->setText(m_note_modified_indicator);
            noteModifiedLabel->setVisible(true);
        }
        else
            noteModifiedLabel->setVisible(false);
    }

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
    if (hasNoteLabel)
    {
        if (state)
        {
            hasNoteLabel->setText(m_note_indicator);
            hasNoteLabel->setVisible(true);
        }
        else
            hasNoteLabel->setHidden(true);
    }
}

