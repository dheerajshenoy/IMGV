#ifndef THUMBNAIL_VIEW_STYLED_ITEM_DELEGATE__HPP
#define THUMBNAIL_VIEW_STYLED_ITEM_DELEGATE__HPP

#include <QStyledItemDelegate>
#include <QPainter>
#include <QApplication>
#include <QIcon>
#include <qnamespace.h>

class ThumbnailViewStyledItemDelegate : public QStyledItemDelegate
{
public:
    using QStyledItemDelegate::QStyledItemDelegate;

    inline void setIconSize(const QSize& size) noexcept
    {
        m_icon_size = size;
    }

    inline QSize iconSize() const noexcept
    {
        return m_icon_size;
    }

    inline void setTextElideMode(const Qt::TextElideMode& mode) noexcept
    {
        m_text_elide_mode = mode;
    }

    inline void setTextElideMode(Qt::TextElideMode&& mode) noexcept
    {
        m_text_elide_mode = mode;
    }

    inline Qt::TextElideMode textElideMode() noexcept
    {
        return m_text_elide_mode;
    }

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override
    {
        painter->save();

        // Check if the item is selected
        if (option.state & QStyle::State_Selected) {
            // Set the selection background color
            painter->fillRect(option.rect, option.palette.highlight());
        } else {
            // Set the normal background color (optional, if needed)
            painter->fillRect(option.rect, option.palette.base());
        }

        // Get the icon and text from the model
        QPixmap pixmap = qvariant_cast<QPixmap>(index.data(Qt::DecorationRole));

        // Center the pixmap horizontally in the item and set it to be above the text
        QRect pixmapRect = option.rect;
        pixmapRect.setSize(pixmap.size());

        pixmapRect.moveTop(option.rect.top() + 5);  // Adjust vertical position for padding
        pixmapRect.moveLeft(option.rect.left() + (option.rect.width() - pixmap.width()) / 2);  // Center horizontally

        // Draw the pixmap
        painter->drawPixmap(pixmapRect, pixmap);

        // Adjust the text rect to be below the icon
        QRect textRect = option.rect;
        textRect.setTop(pixmapRect.bottom() + 5);

        // Set the text color based on selection state
        if (option.state & QStyle::State_Selected) {
            painter->setPen(option.palette.highlightedText().color());
        } else {
            painter->setPen(option.palette.text().color());
        }

        QString text = index.data(Qt::DisplayRole).toString();
        // Elide the text to fit within the available width
        QFontMetrics fontMetrics = painter->fontMetrics();
        QString elidedText = fontMetrics.elidedText(text, m_text_elide_mode, textRect.width());

        // Draw the elided text
        painter->drawText(textRect, Qt::AlignCenter, elidedText);

        painter->restore();
    }

    inline QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override
    {
        // Provide size hint, including space for both icon and text
        QSize textSize = option.fontMetrics.size(Qt::TextSingleLine, index.data(Qt::DisplayRole).toString());
        return QSize(qMax(m_icon_size.width(), textSize.width()), m_icon_size.height() + textSize.height() + 10); // Adjust the size hint as needed
    }

private:
    QSize m_icon_size = QSize(100, 100);
    Qt::TextElideMode m_text_elide_mode = Qt::ElideNone;
};

#endif
