#ifndef StickFigure_H
#define StickFigure_H
#include <QPointF>
#include <QLineF>
#include <QtMath>
#include <stick.h>
#include <QGraphicsScene>
#include <QListWidgetItem>
class StickFigure
{
public:
    StickFigure();
    void  startDrawing(QPointF* point);
    void  previewDrawing(QPointF* point);
    void  endDrawing(QPointF* point);

    void  returnLine();
    void  cancelDrawing();
    void deleteStick(int idx);
    int drawCount;
    int selectStick( QPointF * point);
    QList<stick*>stickList;
    void setLineFromPoint(QPointF *point);
    QLineF lineBuffer;
    stick* currentStick;
    stick* masterStick;
    qreal baseZ;
private:

    QPointF p0;
    QPointF p1;
    stick* stickBuffer;
    QPointF selectOrigin( QPointF * point);
    stick* parentBuffer = NULL;
    QGraphicsScene * myScene ;

};

#endif // StickFigure_H
