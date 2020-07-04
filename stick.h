#ifndef STICK_H
#define STICK_H
#include <QGraphicsItem>
#include <math.h>
#include <style.h>
class stick
{

public:
    stick(QLineF *line);
    QGraphicsLineItem* lineObj(void);
    void setLine(QLineF* line);
    void rotate(QPointF* point);
    stick * parent;
    QList<stick*> children;
    ~stick();
    QPointF getP2(QLineF* line);
    //int parent;
private:
    float angleBuffer;
    void  calcAngle();
    QGraphicsLineItem lineobject;

};

#endif // STICK_H
