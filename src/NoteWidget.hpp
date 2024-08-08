#ifndef NOTE_WIDGET_HPP
#define NOTE_WIDGET_HPP

#include "qmarkdowntextedit/markdownhighlighter.h"
#include <qt6/QtWidgets/QTextEdit>
#include <qt6/QtGui/QKeySequence>
#include <qt6/QtGui/QShortcut>
#include <qt6/QtGui/QShowEvent>
#include <qt6/QtGui/QHideEvent>

class NoteWidget : public QTextEdit
{
    Q_OBJECT
public:
    NoteWidget(QWidget *parent = nullptr);
    ~NoteWidget(){}

signals:
    void saveRequested();
    void modificationChanged(bool);
    void visibilityChanged(bool);

protected:
    void showEvent(QShowEvent *e) noexcept override;
    void hideEvent(QHideEvent *e) noexcept override;

private:
};

#endif
