#include "NoteWidget.hpp"


NoteWidget::NoteWidget(QWidget *parent)
    : QTextEdit(parent)
{
    auto doc = this->document();
    MarkdownHighlighter *highlighter = new MarkdownHighlighter();

    doc->setIndentWidth(4);
    highlighter->setDocument(doc);

    QShortcut *save = new QShortcut(QKeySequence("Ctrl+s"), this);

    connect(save, &QShortcut::activated, this, [&]() {
        emit saveRequested();
    });

    connect(this->document(), &QTextDocument::modificationChanged, this, [&](bool state) { emit modificationChanged(state); });

    this->setPlaceholderText("Type your notes here");
}
void NoteWidget::keyPressEvent(QKeyEvent *e) noexcept
{
    if (e->key() == Qt::Key_Escape)
        this->clearFocus();
    else
        QTextEdit::keyPressEvent(e);
}
