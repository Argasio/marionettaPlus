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
    ~stick();
    int parent;
private:
    float angleBuffer;
    void  calcAngle();
    QGraphicsLineItem* lineobject;

};

#endif // STICK_H
