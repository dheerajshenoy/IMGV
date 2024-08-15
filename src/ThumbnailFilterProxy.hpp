#ifndef THUMBNAIL_FILTER_PROXY_HPP
#define THUMBNAIL_FILTER_PROXY_HPP

#include <QSortFilterProxyModel>
#include "Thumbnail.hpp"

class ThumbnailFilterProxy : public QSortFilterProxyModel
{
    Q_OBJECT

    public:
    ThumbnailFilterProxy(QObject *parent = nullptr) : QSortFilterProxyModel(parent) {
    }

    void setFilterText(const QString &text)
    {
        filterText = text;
        qDebug() << filterText;
        invalidateFilter();
    }

    void setSearchRole(const Thumbnail::Data role)
    {
        m_role = role;
    }


protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override
    {
        if (filterText.isEmpty())
            return true;
        QString data;

        QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);

        if (m_role == Thumbnail::Tag)
        {
            qDebug() << "TAG ROLE";
            data = sourceModel()->data(index, Thumbnail::Tag).toString();
        }

        else if (m_role == Thumbnail::FileName)
            data = sourceModel()->data(index, Thumbnail::FileName).toString();

        else if (m_role == Thumbnail::Note)
            data = sourceModel()->data(index, Thumbnail::Note).toString();


        QRegularExpression regex(filterText, QRegularExpression::CaseInsensitiveOption);
        return regex.match(data).hasMatch();
    }

private:
    QString filterText;

    Thumbnail::Data m_role;
};

#endif
