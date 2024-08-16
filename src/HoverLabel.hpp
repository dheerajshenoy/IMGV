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

    inline void setShowText(const QString text) noexcept
    {
        m_show_text = text;
        this->setText(m_show_text);
    }

    inline void setFullText(const QString text) noexcept
    {
        m_full_text = text;
    }

protected:
    inline void enterEvent(QEnterEvent *e) noexcept override
    {
        this->setText(m_full_text);
    }

    inline void leaveEvent(QEvent *e) noexcept override
    {
        this->setText(m_show_text);
    }


private:
    QString m_full_text, m_show_text;

};

#endif
