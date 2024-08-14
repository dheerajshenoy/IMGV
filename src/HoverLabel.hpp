#ifndef HOVER_LABEL__HPP
#define HOVER_LABEL__HPP

#include <QLabel>
#include <QWidget>
#include <QEnterEvent>

class HoverLabel : public QLabel
{

public:
    HoverLabel(QWidget *parent = nullptr);
    ~HoverLabel() {}

    void setFullText(const QString full_text) noexcept;
    void setShowText(const QString show_text) noexcept;

protected:
    void enterEvent(QEnterEvent *e) noexcept override;
    void leaveEvent(QEvent *e) noexcept override;


private:
    QString m_full_text, m_show_text;

};

#endif
