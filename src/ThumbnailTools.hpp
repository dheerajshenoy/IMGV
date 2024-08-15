#ifndef THUMBNAIL_TOOLS__HPP
#define THUMBNAIL_TOOLS__HPP

#include <QWidget>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>

class ThumbnailTools : public QWidget
{

    Q_OBJECT
public:
    ThumbnailTools(QWidget *parent = nullptr);
    ~ThumbnailTools() {}

signals:
    void search();
    void filter();
    void resetFilter();


private:
    QHBoxLayout *m_layout = new QHBoxLayout();
    QPushButton *m_search_btn = new QPushButton(QIcon(":/resources/images/search.svg"), "");
    QPushButton *m_filter_tag_btn = new QPushButton(QIcon(":/resources/images/filter.svg"), "");
    QPushButton *m_reset_filter_btn = new QPushButton(QIcon(":/resources/images/filter_reset.svg"), "");
};

#endif
