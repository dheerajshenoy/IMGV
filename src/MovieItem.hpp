#ifndef MOVIE_ITEM_HPP
#define MOVIE_ITEM_HPP

#include <qt6/QtWidgets/QGraphicsItem>
#include <qt6/QtCore/QObject>
#include <qt6/QtGui/QMovie>
#include <qt6/QtGui/QPainter>

class MovieItem : public QGraphicsItem
{
public:
    using QGraphicsItem::QGraphicsItem;

    void setMovie(QMovie* movie)
    {
        prepareGeometryChange();
        QObject::disconnect(mConnection); // disconnect old object
        mMovie = movie;
        if (mMovie)
            mConnection = QObject::connect(mMovie, &QMovie::frameChanged, [=]{ update(); });
    }

    QRectF boundingRect() const override
    {
        if (mMovie)
            return mMovie->frameRect();
        else
            return QRectF();
    }

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override
    {
        if (mMovie)
            painter->drawPixmap(mMovie->frameRect(), mMovie->currentPixmap(), mMovie->frameRect());
    }

private:
    QPointer<QMovie> mMovie;
    QMetaObject::Connection mConnection;
};

#endif
