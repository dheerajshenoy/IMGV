#include "NoteWidget.hpp"


NoteWidget::NoteWidget(QWidget *parent)
    : QTextEdit(parent)
{
    auto doc = this->document();
    MarkdownHighlighter *highlighter = new MarkdownHighlighter();
    highlighter->setDocument(doc);
}
