#include "HoverLabel.hpp"

HoverLabel::HoverLabel(QWidget *parent)
    : QLabel(parent)
{
    setMouseTracking(true);
    this->setTextInteractionFlags(Qt::TextInteractionFlag::TextSelectableByMouse | Qt::TextInteractionFlag::TextSelectableByKeyboard);
    this->setText(m_show_text);
}

void HoverLabel::enterEvent(QEnterEvent *e) noexcept
{
    this->setText(m_full_text);
}

void HoverLabel::leaveEvent(QEvent *e) noexcept
{
    this->setText(m_show_text);
}

void HoverLabel::setShowText(const QString text) noexcept
{
    m_show_text = text;
    this->setText(m_show_text);
}

void HoverLabel::setFullText(const QString text) noexcept
{
    m_full_text = text;
}
