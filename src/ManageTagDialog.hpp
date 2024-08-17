#ifndef MANAGE_TAG_DIALOG__HPP
#define MANAGE_TAG_DIALOG__HPP

#include <QDialog>
#include <QFormLayout>
#include <QComboBox>
#include <QHideEvent>
#include <QShowEvent>
#include <QPushButton>
#include "Thumbnail.hpp"

class ManageTagDialog : public QDialog
{
    Q_OBJECT
public:
    ManageTagDialog(const QStringList &tags, QVector<Thumbnail> &thumbnails, QWidget *parent = nullptr);
    void renameTag() noexcept;
    void deleteTag() noexcept;

signals:
    void visibilityChanged(bool);

protected:

    inline void showEvent(QShowEvent *e) noexcept override
    {
        emit visibilityChanged(isVisible());
    }

    inline void hideEvent(QHideEvent *e) noexcept override
    {
        emit visibilityChanged(isVisible());
    }

private:
    QFormLayout *m_layout = new QFormLayout();
    QComboBox *m_tags_combo_box = new QComboBox();
    QVector<Thumbnail> m_thumbnails;
    QPushButton *m_rename_btn = new QPushButton("Rename");
    QPushButton *m_delete_btn = new QPushButton("Delete");
    QPushButton *m_done_btn = new QPushButton("Done");
};

#endif
