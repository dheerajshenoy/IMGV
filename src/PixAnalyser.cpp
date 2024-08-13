#include "PixAnalyser.hpp"

PixAnalyser::PixAnalyser(QWidget *parent)
    : QWidget(parent)
{
    this->setLayout(m_layout);

    this->setWindowFlag(Qt::WindowType::Dialog);

    m_layout->addWidget(m_color, 0, 0, 1, 2);
    m_layout->addWidget(new QLabel("HEX: "), 1, 0);
    m_layout->addWidget(m_color_name, 1, 1);
    m_layout->addWidget(new QLabel("RGB: "), 2, 0);
    m_layout->addWidget(m_color_rgb, 2, 1);
    m_layout->addWidget(new QLabel("HSV: "), 3, 0);
    m_layout->addWidget(m_color_hsv, 3, 1);
    m_layout->setAlignment(Qt::AlignmentFlag::AlignCenter);

    m_color_name->setTextInteractionFlags(Qt::TextInteractionFlag::TextSelectableByMouse | Qt::TextInteractionFlag::TextSelectableByKeyboard);
    m_color_rgb->setTextInteractionFlags(Qt::TextInteractionFlag::TextSelectableByMouse | Qt::TextInteractionFlag::TextSelectableByKeyboard);
    m_color_hsv->setTextInteractionFlags(Qt::TextInteractionFlag::TextSelectableByMouse | Qt::TextInteractionFlag::TextSelectableByKeyboard);

}

void PixAnalyser::setPixmap(const QPixmap &pix) noexcept
{
    m_img = pix.toImage();
}

void PixAnalyser::showEvent(QShowEvent *e) noexcept
{
    emit visibilityChanged(true);
}

void PixAnalyser::hideEvent(QHideEvent *e) noexcept
{
    emit visibilityChanged(false);
}

void PixAnalyser::analysePix(const QPointF &loc) noexcept
{
    QColor color = QColor(m_img.pixelColor(loc.x(), loc.y()));
    m_color_name->setText(color.name());
    m_color_rgb->setText(QString("(%1, %2, %3)").arg(color.red()).arg(color.green()).arg(color.blue()));

    QColor hsv = color.toHsv();
    m_color_hsv->setText(QString("(%1, %2, %3)").arg(hsv.hue()).arg(hsv.saturation()).arg(hsv.value()));

    m_color->setStyleSheet(QString("background-color: %1").arg(color.name()));
}
