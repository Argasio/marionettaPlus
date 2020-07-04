#ifndef StickFigure_H
#define StickFigure_H
#include <QPointF>
#include <QLineF>
#include <QtMath>
#include <stick.h>
#include <QGraphicsScene>
class StickFigure
{
public:
    StickFigure();
    QGraphicsLineItem * startDrawing(QPointF* point);
    QGraphicsLineItem * previewDrawing(QPointF* point);
    QGraphicsLineItem * endDrawing(QPointF* point);

    QGraphicsLineItem * returnLine();
    void  cancelDrawing();
    void deleteStick(int idx);
    int drawCount;
    QList<stick*>stickList;
    QList<QGraphicsItem*>itemList;
    QLineF lineBuffer;
private:

    QPointF p0;
    QPointF p1;
    stick* stickBuffer;
    QPointF selectOrigin( QPointF * point);
    stick* parentBuffer = NULL;
    QGraphicsScene * myScene ;

};

#endif // StickFigure_H
