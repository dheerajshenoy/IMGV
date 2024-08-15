#include "ThumbnailTools.hpp"


ThumbnailTools::ThumbnailTools(QWidget *parent)
    : QWidget(parent)
{
    this->setLayout(m_layout);
    m_layout->setSpacing(10);
    m_layout->addWidget(m_search_btn);
    m_layout->addWidget(m_filter_tag_btn);
    m_layout->addWidget(m_reset_filter_btn);
    m_layout->addStretch(1);

    m_search_btn->setToolTip("Search Thumbnails");
    m_filter_tag_btn->setToolTip("Filter by tags");
    m_reset_filter_btn->setToolTip("Reset Filter");

    connect(m_search_btn, &QPushButton::clicked, this, [&]() { emit search(); });
    connect(m_filter_tag_btn, &QPushButton::clicked, this, [&]() { emit filter(); });
    connect(m_reset_filter_btn, &QPushButton::clicked, this, [&]() { emit resetFilter(); });
}
