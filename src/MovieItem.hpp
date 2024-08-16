#ifndef MOVIE_ITEM_HPP
#define MOVIE_ITEM_HPP

#include <QGraphicsItem>
#include <QObject>
#include <QMovie>
#include <QPainter>

class MovieItem : public QGraphicsItem
{
public:
    using QGraphicsItem::QGraphicsItem;

    void setMovie(QMovie* movie) noexcept
    {
        prepareGeometryChange();
        QObject::disconnect(mConnection); // disconnect old object
        mMovie = movie;
        if (mMovie)
            mConnection = QObject::connect(mMovie, &QMovie::frameChanged, [&]{ update(); });
    }

    inline QRectF boundingRect() const noexcept override 
    {
        if (mMovie)
            return mMovie->frameRect();
        else
            return QRectF();
    }

    inline void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) noexcept override
    {
        if (mMovie)
            painter->drawPixmap(mMovie->frameRect(), mMovie->currentPixmap(), mMovie->frameRect());
    }

private:
    QPointer<QMovie> mMovie;
    QMetaObject::Connection mConnection;
};

#endif
