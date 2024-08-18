#ifndef THUMBNAIL_TOOLS__HPP
#define THUMBNAIL_TOOLS__HPP

#include <QWidget>
#include <QBoxLayout>
#include <QPushButton>
#include <QLabel>

class ThumbnailTools : public QWidget
{

    Q_OBJECT
public:
    explicit ThumbnailTools(Qt::Orientation&& orientation, QWidget *parent = nullptr);

signals:
    void search();
    void filter();
    void resetFilter();


private:
    QBoxLayout *m_layout;
    QPushButton *m_search_btn = new QPushButton(QIcon(":/resources/images/search.svg"), "");
    QPushButton *m_filter_tag_btn = new QPushButton(QIcon(":/resources/images/filter.svg"), "");
    QPushButton *m_reset_filter_btn = new QPushButton(QIcon(":/resources/images/filter_reset.svg"), "");
};

#endif
