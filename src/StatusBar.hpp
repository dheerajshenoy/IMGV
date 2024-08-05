#ifndef STATUSBAR_HPP
#define STATUSBAR_HPP

#include <qt6/QtWidgets/QWidget>
#include <qt6/QtWidgets/QHBoxLayout>
#include <qt6/QtWidgets/QLabel>
#include <qt6/QtCore/QFileInfo>
#include <qt6/QtGui/QPixmap>

class StatusBar : public QWidget
{
public:
    explicit StatusBar(QWidget *parent = nullptr);

    void updateFileInfo(const QString &filePath);

private:
    QHBoxLayout *layout;
    QLabel *filePathLabel;
    QLabel *fileSizeLabel;
    QLabel *imageDimensionsLabel;

    QString formatFileSize(qint64 size) const;
};

#endif
