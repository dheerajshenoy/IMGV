#ifndef THUMBNAIL_FILTER_PROXY_HPP
#define THUMBNAIL_FILTER_PROXY_HPP

#include <QSortFilterProxyModel>
#include "Thumbnail.hpp"

class ThumbnailFilterProxy : public QSortFilterProxyModel
{
    Q_OBJECT

    public:
    ThumbnailFilterProxy(QObject *parent = nullptr) : QSortFilterProxyModel(parent)
    {}

    inline void setFilterText(const QString &text) noexcept
    {
        m_filterText = text;
        invalidateFilter();
    }

    inline void setSearchRole(const Thumbnail::Data& role) noexcept
    {
        m_role = role;
    }


protected:
    bool filterAcceptsRow(const int sourceRow, const QModelIndex &sourceParent) const noexcept override
    {
        if (m_filterText.isEmpty())
            return true;
        QString data;

        QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);

        if (m_role == Thumbnail::Tag)
            data = sourceModel()->data(index, Thumbnail::Tag).toString();

        else if (m_role == Thumbnail::FileName)
            data = sourceModel()->data(index, Thumbnail::FileName).toString();

        else if (m_role == Thumbnail::Note)
            data = sourceModel()->data(index, Thumbnail::Note).toString();

        QRegularExpression regex(m_filterText, QRegularExpression::CaseInsensitiveOption);
        return regex.match(data).hasMatch();
    }

private:
    QString m_filterText;
    Thumbnail::Data m_role;
};

#endif
