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

    void updateFileInfo(const QString &filePath) const noexcept;
    void setSessionName(const QString&) const noexcept;
    void clearTexts() const noexcept;
    void setMsg(const QString &msg, const int &sec = 1) noexcept;
    void setMsg(QString &&msg, const int &sec = 1) noexcept;
    void setNoteModified(const bool &state) noexcept;
    void modificationLabelVisiblity(const bool &state) noexcept;
    void setHasNote(const bool &state) noexcept;
    void defaultLayout() noexcept;
    void addWidget(const QString &widget) noexcept;
    void addWidgets(const QStringList &widget) noexcept;

    inline void setImgDimension(const int& w, const int& h) const noexcept
    {
        if (imageDimensionsLabel)
            imageDimensionsLabel->setText(QString("(%1,%2)").arg(w).arg(h));
    }

    inline void setSpacing(int&& spacing) const noexcept
    {
        layout->setSpacing(spacing);
    }

    inline void setSpacing(const int& spacing) const noexcept
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

    inline void setZoom(const QString &zoom) const noexcept
    {
        if (zoomLabel)
            zoomLabel->setText(zoom + " %");
    }

    inline void hideMsg() const noexcept
    {
        msgLabel->hide();
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
    QTimer m_timer;
};

#endif
