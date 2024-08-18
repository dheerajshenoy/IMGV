#include "PixAnalyser.hpp"
#include <numbers>

PixAnalyser::PixAnalyser(QWidget *parent)
    : QWidget(parent)
{
    this->setLayout(m_layout);
    this->setWindowFlag(Qt::WindowType::Dialog);

    m_layout->addRow(m_color);
    m_layout->addRow("HEX:", m_color_hex);
    m_layout->addRow("RGBA:", m_color_rgb);
    m_layout->addRow("HSVA:", m_color_hsv);
    m_layout->addRow("CMYK:", m_color_cmyk);
    m_layout->addRow("HSLA:", m_color_hsl);
    m_layout->addRow(m_copy_info);
    m_layout->addRow(m_pick_btn);
    m_layout->addRow(m_done_btn);

    m_pick_btn->setVisible(false);

    connect(m_pick_btn, &QPushButton::clicked, this, [&]() {
        m_pick_btn->setVisible(false);
        emit pickColor();
    });

    connect(m_done_btn, &QPushButton::clicked, this, &QWidget::close);

}

void PixAnalyser::analysePix(const QPointF &loc) const noexcept
{
    auto x = loc.x();
    auto y = loc.y();
    if ( (x < 0 || x > m_img.width()) || (y < 0 || y > m_img.height()) ) return;

    QColor color = QColor(m_img.pixelColor(x, y));
    m_color_hex->setText(color.name());
    m_color_rgb->setText(QString("%1, %2, %3, %4").arg(color.red()).arg(color.green()).arg(color.blue()).arg(color.alpha()));

    QColor hsv = color.toHsv();
    m_color_hsv->setText(QString("%1, %2, %3, %4").arg(hsv.hue()).arg(hsv.saturation()).arg(hsv.value()).arg(hsv.alpha()));

    QColor cmyk = color.toCmyk();
    m_color_cmyk->setText(QString("%1, %2, %3, %4").arg(cmyk.cyan()).arg(cmyk.magenta()).arg(cmyk.yellow()).arg(cmyk.black()));

    QColor hsl = color.toHsl();
    m_color_hsl->setText(QString("%1, %2, %3, %4").arg(hsl.hue()).arg(hsl.saturation()).arg(hsl.lightness()).arg(hsl.alpha()));

    m_color->setStyleSheet(QString("background-color: %1").arg(color.name()));
}
