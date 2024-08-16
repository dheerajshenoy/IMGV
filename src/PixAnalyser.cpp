#include "PixAnalyser.hpp"
#include <numbers>

PixAnalyser::PixAnalyser(QWidget *parent)
    : QWidget(parent)
{
    this->setLayout(m_layout);
    this->setWindowFlag(Qt::WindowType::Dialog);

    m_layout->addRow(m_color);
    m_layout->addRow("HEX:", m_color_name);
    m_layout->addRow("RGB:", m_color_rgb);
    m_layout->addRow("HSV:", m_color_hsv);
    m_layout->addRow(m_pick_btn);
    m_layout->addRow(m_done_btn);

    m_pick_btn->setVisible(false);

    connect(m_pick_btn, &QPushButton::clicked, this, [&]() {
        m_pick_btn->setVisible(false);
        emit pickColor();
    });

    connect(m_done_btn, &QPushButton::clicked, this, &QWidget::close);

    m_color_name->setTextInteractionFlags(Qt::TextInteractionFlag::TextSelectableByMouse | Qt::TextInteractionFlag::TextSelectableByKeyboard);
    m_color_rgb->setTextInteractionFlags(Qt::TextInteractionFlag::TextSelectableByMouse | Qt::TextInteractionFlag::TextSelectableByKeyboard);
    m_color_hsv->setTextInteractionFlags(Qt::TextInteractionFlag::TextSelectableByMouse | Qt::TextInteractionFlag::TextSelectableByKeyboard);
}

void PixAnalyser::analysePix(const QPointF &loc) noexcept
{
    auto x = loc.x();
    auto y = loc.y();
    if ( (x < 0 || x > m_img.width()) || (y < 0 || y > m_img.height()) ) return;

    QColor color = QColor(m_img.pixelColor(x, y));
    m_color_name->setText(color.name());
    m_color_rgb->setText(QString("(%1, %2, %3)").arg(color.red()).arg(color.green()).arg(color.blue()));

    QColor hsv = color.toHsv();
    m_color_hsv->setText(QString("(%1, %2, %3)").arg(hsv.hue()).arg(hsv.saturation()).arg(hsv.value()));

    m_color->setStyleSheet(QString("background-color: %1").arg(color.name()));
}

