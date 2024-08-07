#ifndef STATUSBAR_HPP
#define STATUSBAR_HPP

#include <qt6/QtWidgets/QWidget>
#include <qt6/QtWidgets/QHBoxLayout>
#include <qt6/QtWidgets/QLabel>
#include <qt6/QtCore/QFileInfo>
#include <qt6/QtCore/QTimer>
#include <qt6/QtGui/QPixmap>
#include <qt6/QtGui/QShowEvent>
#include <qt6/QtGui/QHideEvent>

class StatusBar : public QWidget
{
    Q_OBJECT
public:
    explicit StatusBar(QWidget *parent = nullptr);

    void updateFileInfo(const QString &filePath);
    void setSessionName(QString) noexcept;
    void clearTexts() noexcept;
    void setMsg(QString msg, int sec = 1) noexcept;

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
    QLabel *sessionLabel = new QLabel("Session:");

    QString formatFileSize(qint64 size) const;
};

#endif
