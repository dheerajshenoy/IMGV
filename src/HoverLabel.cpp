#include "HoverLabel.hpp"

HoverLabel::HoverLabel(QWidget *parent)
    : QLabel(parent)
{
    setMouseTracking(true);
    this->setTextInteractionFlags(Qt::TextInteractionFlag::TextSelectableByMouse |
                                  Qt::TextInteractionFlag::TextSelectableByKeyboard);
    this->setText(m_show_text);
}

