#ifndef STATUSBAR_HPP
#define STATUSBAR_HPP

#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QFileInfo>
#include <QTimer>
#include <QPixmap>
#include <QShowEvent>
#include <QHideEvent>
#include "HoverLabel.hpp"

class StatusBar : public QWidget
{
    Q_OBJECT
public:
    explicit StatusBar(QWidget *parent = nullptr);

    void updateFileInfo(const QString &filePath) noexcept;
    void setSessionName(QString) noexcept;
    void clearTexts() noexcept;
    void setMsg(QString msg, int sec = 1) noexcept;
    void setNoteModified(bool state) noexcept;
    void modificationLabelVisiblity(bool state) noexcept;
    void setHasNote(bool state) noexcept;
    void defaultLayout() noexcept;
    void addWidget(const QString &widget) noexcept;

    inline void setImgDimension(const int w, const int h) noexcept
    {
        if (imageDimensionsLabel)
            imageDimensionsLabel->setText(QString("(%1,%2)").arg(w).arg(h));
    }

    inline void setSpacing(const int spacing) noexcept
    {
        layout->setSpacing(spacing);
    }

    inline void setNoteIndicator(const QString &indicator) noexcept
    {
        m_note_indicator = indicator;
    }

    inline void setNoteModifiedIndicator(const QString &indicator) noexcept
    {
        m_note_modified_indicator = indicator;
    }

    inline void setZoom(const QString &zoom) noexcept
    {
        if (zoomLabel)
            zoomLabel->setText(zoom + " %");
    }


signals:
    void visibilityChanged(bool);

protected:
    inline void hideEvent(QHideEvent *e) noexcept override
    {
        emit visibilityChanged(isVisible());
    }

    inline void showEvent(QShowEvent *e) noexcept override
    {
        emit visibilityChanged(isVisible());
    }


private:
    QHBoxLayout *layout = nullptr;
    QLabel *msgLabel = nullptr;
    /*QLabel *filePathLabel;*/
    QLabel *fileSizeLabel = nullptr;
    QLabel *imageDimensionsLabel = nullptr;
    QLabel *sessionLabel = nullptr;
    QLabel *noteModifiedLabel = nullptr;
    QLabel *hasNoteLabel = nullptr;
    QLabel *zoomLabel = nullptr;
    HoverLabel *hoverFilePathLabel = nullptr;
    QString formatFileSize(qint64 size) const;
    QString m_note_indicator = "NOTE";
    QString m_note_modified_indicator = "[M]";
};

#endif
