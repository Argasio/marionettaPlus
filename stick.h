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
//-------- methods ----------//
    stick(QLineF *line);
    stick(stick *S);
    stick();
    ~stick();
    stick * parent = nullptr;
    QLineF myLine;
    QPointF getP2(QLineF* line);
    QRectF boundingRect() const ;
    QRectF br;
    QImage calcImg();
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void refresh(int mode);
    void endRotation();
    void setLine(QLineF* line);
    void rotate(QPointF* point);
    void manipulate(QPointF* point);
//-------- variables----------//
    bool highlight = false;
    bool selected = false;
    bool stepchild = false;
    bool master = false;
    int parentIdx = 0;
    int version = 1;
    int type = stickType::LINE;
    float imgAngle = 0;
    float imgWScale = 1;
    float imgHScale = 1;
    int imgAngleOffset = 0;
    int idx = 0;
    QPointF imgOffset = QPointF(0,0);
    QRectF imgRect;
    QList<stick*> children;
    QPen Pen;
    QBrush Brush;
    qreal Z;
    QImage *stickImg = nullptr;

private:
    //-------- methods----------//
    QRectF updateBr(int mode);
    void  calcAngle();
    //-------- variables----------//
    float angleBuffer;

};
void sceneRemover(QGraphicsScene *sceneToClear);
QRectF calcImgRect(QLineF l,QSizeF s);
#endif // STICK_H
