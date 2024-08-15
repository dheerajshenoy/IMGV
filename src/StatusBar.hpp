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

    void updateFileInfo(const QString &filePath);
    void setSessionName(QString) noexcept;
    void clearTexts() noexcept;
    void setMsg(QString msg, int sec = 1) noexcept;
    void setImgDimension(const int w, const int h) noexcept;
    void setNoteModified(bool state) noexcept;
    void modificationLabelVisiblity(bool state) noexcept;
    void setHasNote(bool state) noexcept;
    void defaultLayout() noexcept;
    void addWidget(const QString &widget) noexcept;
    void setSpacing(const int) noexcept;
    void setNoteIndicator(const QString &indicator) noexcept;
    void setNoteModifiedIndicator(const QString &indicator) noexcept;
    void setZoom(const QString &zoom) noexcept;

signals:
    void visibilityChanged(bool);

protected:
    void showEvent(QShowEvent *e) noexcept override;
    void hideEvent(QHideEvent *e) noexcept override;

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
