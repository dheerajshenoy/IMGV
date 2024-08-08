#include "NoteWidget.hpp"


NoteWidget::NoteWidget(QWidget *parent)
    : QTextEdit(parent)
{
    auto doc = this->document();
    MarkdownHighlighter *highlighter = new MarkdownHighlighter();
    highlighter->setDocument(doc);

    QShortcut *save = new QShortcut(QKeySequence("Ctrl+s"), this);

    connect(save, &QShortcut::activated, this, [&]() {
        emit saveRequested();
    });

    connect(this->document(), &QTextDocument::modificationChanged, this, [&](bool state) { emit modificationChanged(state); });

}

void NoteWidget::showEvent(QShowEvent *e) noexcept
{
    emit visibilityChanged(true);
}

void NoteWidget::hideEvent(QHideEvent *e) noexcept
{
    emit visibilityChanged(false);
}
