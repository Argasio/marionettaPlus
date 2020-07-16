#ifndef STICK_H
#define STICK_H
#include <QGraphicsItem>
#include <math.h>
#include <style.h>
#include <QPainter>
class stick:public QGraphicsItem
{

public:
    stick(QLineF *line);
    void setLine(QLineF* line);
    void rotate(QPointF* point);
    stick * parent;
    QList<stick*> children;
    ~stick();
    QLineF myLine;
    QPointF getP2(QLineF* line);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QRectF boundingRect() const ;
    QRectF br;
    void refresh(int mode);
    void endRotation();
    bool master;
    QPen Pen;
    qreal Z;
    bool highlight;
    //StickFigure* myStickFigure;
    //int parent;
private:
    float angleBuffer;
    QRectF updateBr(int mode);
    void  calcAngle();
};

#endif // STICK_H
