#ifndef THUMBNAIL_FILTER_PROXY_HPP
#define THUMBNAIL_FILTER_PROXY_HPP

#include <QSortFilterProxyModel>

class ThumbnailFilterProxy : public QSortFilterProxyModel
{
    Q_OBJECT

    public:
    ThumbnailFilterProxy(QObject *parent = nullptr) : QSortFilterProxyModel(parent) {
    }

    void setFilterText(const QString &text)
    {
        filterText = text;
        invalidateFilter();
    }

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override
    {
        if (filterText.isEmpty())
            return true;

        QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);
        QString data = sourceModel()->data(index).toString();

        QRegularExpression regex(filterText, QRegularExpression::CaseInsensitiveOption);
        return regex.match(data).hasMatch();
    }

private:
    QString filterText;
};

#endif
