#ifndef STICK_H
#define STICK_H
#include <QGraphicsItem>
#include <math.h>
#include <style.h>
#include <QPainter>
#include <QGraphicsScene>

#define STEPCHILD -1
extern bool onionRender;
class StickFigure;
class stick:public QGraphicsItem
{

public:
    enum stickType{
        LINE = 0,
        CIRCLE = 1,
        IMAGE = 2,
        RECT = 3,
        TAPER = 4,
        TRAPEZOID = 5
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
    int rotationSpinboxBuffer = 0;
    int scaleSpinBoxBuffer = 100;
    int version = 1;
    int type = stickType::LINE;
    float imgAngle = 0;
    float imgWScale = 1;
    float imgHScale = 1;
    float scaleBuffer = 1;
    float widthBuffer = 1;
    int imgAngleOffset = 0;
    int idx = 0;
    float angleBuffer = 0; //usato per disegnare lo stick nei refresh
    float angleBuffer2 = 0; //usato nella rotazione di tutto lo stickfigure
    QPointF imgOffset = QPointF(0,0);
    QRectF imgRect;
    QList<stick*> children;
    QList<void*> linkedStickFigure;
    QPen Pen;
    QBrush Brush;
    qreal Z;
    QImage *stickImg = nullptr;
    StickFigure* myStickFigure = nullptr;
    void rotate(float angle);
    void scale(float scaleFactor);
private:
    //-------- methods----------//
    QRectF updateBr(int mode);
    void  calcAngle();
    //-------- variables----------//


};
void sceneRemover(QGraphicsScene *sceneToClear);
QRectF calcImgRect(QLineF l,QSizeF s);
#endif // STICK_H
