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

signals:
    void visibilityChanged(bool);

protected:
    void showEvent(QShowEvent *e) noexcept override;
    void hideEvent(QHideEvent *e) noexcept override;

private:
    QHBoxLayout *layout;
    QLabel *msgLabel;
    QLabel *filePathLabel;
    QLabel *fileSizeLabel;
    QLabel *imageDimensionsLabel;
    QLabel *sessionLabel = new QLabel("No Session");
    QLabel *noteModifiedLabel = new QLabel();
    QLabel *hasNoteLabel = new QLabel();

    QString formatFileSize(qint64 size) const;
    QString m_note_indicator = "NOTE";
    QString m_note_modified_indicator = "[M]";
};

#endif
