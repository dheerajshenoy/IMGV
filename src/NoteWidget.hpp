#ifndef NOTE_WIDGET_HPP
#define NOTE_WIDGET_HPP

#include "qmarkdowntextedit/markdownhighlighter.h"
#include <QTextEdit>
#include <QKeySequence>
#include <QShortcut>
#include <QShowEvent>
#include <QKeyEvent>
#include <QHideEvent>

class NoteWidget : public QTextEdit
{
    Q_OBJECT
public:
    NoteWidget(QWidget *parent = nullptr);
    ~NoteWidget(){}
    inline void setModified(const bool state) noexcept
    {
        this->document()->setModified(state);
    }

signals:
    void saveRequested();
    void modificationChanged(bool);
    void visibilityChanged(bool);





protected:
    void keyPressEvent(QKeyEvent *e) noexcept override;


    inline void showEvent(QShowEvent *e) noexcept override
    {
        emit visibilityChanged(true);
    }

    inline void hideEvent(QHideEvent *e) noexcept override
    {
        emit visibilityChanged(false);
    }

};

#endif
