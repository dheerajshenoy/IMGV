#include "ManageTagDialog.hpp"


ManageTagDialog::ManageTagDialog(const QStringList &tags, QVector<Thumbnail> &thumbnails, QWidget *parent)
    : m_thumbnails(thumbnails), QDialog(parent)
{
    this->setLayout(m_layout);

    m_layout->addRow("&Tag:", m_tags_combo_box);
    m_tags_combo_box->addItems(tags);

    QHBoxLayout *m_btn_layout = new QHBoxLayout();
    m_btn_layout->addWidget(m_rename_btn);
    m_btn_layout->addWidget(m_done_btn);
    m_btn_layout->addWidget(m_delete_btn);
    m_layout->addRow(m_btn_layout);

    connect(m_done_btn, &QPushButton::clicked, this, &QDialog::done);
}

void ManageTagDialog::renameTag() noexcept
{


}

void ManageTagDialog::deleteTag() noexcept
{


}

void ManageTagDialog::showEvent(QShowEvent *e) noexcept
{
    emit visibilityChanged(isVisible());
}

void ManageTagDialog::hideEvent(QHideEvent *e) noexcept
{
    emit visibilityChanged(isVisible());
}
