#ifndef THUMBNAIL_WIDGET_HPP
#define THUMBANIL_WIDGET_HPP

#include <qt6/QtWidgets/QListWidget>
#include "Thumbnail.hpp"

class ThumbnailWidget : public QListWidget
{
    Q_OBJECT
public:
    explicit ThumbnailWidget(QWidget *parent = nullptr);
    ~ThumbnailWidget(){}

    void createThumbnails(const QStringList &fileNames);

private:
    QPixmap createThumbnail(const QString &fileName, const QSize &size);
};

#endif
