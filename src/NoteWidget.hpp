#ifndef NOTE_WIDGET_HPP
#define NOTE_WIDGET_HPP

#include "qmarkdowntextedit/markdownhighlighter.h"
#include <qt6/QtWidgets/QTextEdit>

class NoteWidget : public QTextEdit
{
public:
    NoteWidget(QWidget *parent = nullptr);
    ~NoteWidget(){}

private:
};

#endif
