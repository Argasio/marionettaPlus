#ifndef STICK_H
#define STICK_H
#include <QGraphicsItem>
#include <math.h>
#include <style.h>
#include <QPainter>
#include <QGraphicsScene>
#define STEPCHILD -1
extern bool onionRender;

class stick:public QGraphicsItem
{

public:
    enum stickType{
        LINE = 0,
        CIRCLE = 1,
        IMAGE = 2
    };
    stick(QLineF *line);
    void setLine(QLineF* line);
    void rotate(QPointF* point);
    stick * parent = nullptr;
    QList<stick*> children;
    int type = stickType::LINE;
    ~stick();
    QLineF myLine;
    QPointF getP2(QLineF* line);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QRectF boundingRect() const ;
    QRectF br;
    int idx = 0;
    void refresh(int mode);
    void endRotation();
    bool master = false;
    QPen Pen;
    qreal Z;
    QPixmap *stickImg = nullptr;
    bool highlight = false;
    bool stepchild = false;
    //StickFigure* myStickFigure;
    int parentIdx = 0;
    int version = 1;
    stick(stick *S);
    stick();
private:
    float angleBuffer;
    QRectF updateBr(int mode);
    void  calcAngle();
};
void sceneRemover(QGraphicsScene *sceneToClear);
#endif // STICK_H
