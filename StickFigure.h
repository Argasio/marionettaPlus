#ifndef StickFigure_H
#define StickFigure_H
#include <QPointF>
#include <QLineF>
#include <QtMath>
#include <stick.h>
class StickFigure
{
public:
    StickFigure();
    QGraphicsLineItem * startDrawing(QPointF* point);
    QGraphicsLineItem * previewDrawing(QPointF* point);
    QGraphicsLineItem * endDrawing(QPointF* point);
    QGraphicsLineItem * returnLine();
    void  cancelDrawing();
    int drawCount;
    QList<stick*>stickList;
    QList<QGraphicsItem*>itemList;
private:
    QLineF lineBuffer;
    QPointF p0;
    QPointF p1;
    stick* stickBuffer;
    QPointF selectOrigin( QPointF * point);
    int parentBuffer;

};

#endif // StickFigure_H
