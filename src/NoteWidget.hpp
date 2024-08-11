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
    void setModified(bool) noexcept;

signals:
    void saveRequested();
    void modificationChanged(bool);
    void visibilityChanged(bool);


protected:
    void showEvent(QShowEvent *e) noexcept override;
    void hideEvent(QHideEvent *e) noexcept override;
    void keyPressEvent(QKeyEvent *e) noexcept override;

private:
};

#endif
