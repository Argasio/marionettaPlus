#include "stick.h"
#include <QGraphicsItem>

//lo stick si basa fondamentalmente su un QGraphicsLineObject, uno stick alloca una lineobject
stick::stick(QLineF *line)
{
    lineobject = new QGraphicsLineItem();
    lineobject->setPen(myPen);
    lineobject->setFlag(QGraphicsItem::ItemIsSelectable,true);
    setLine(line);
}
//lo stick misura lángolo effettivo dell'oggetto linea rispetto al piano di disegno
void stick::calcAngle()
{
    angleBuffer = (180/M_PI)*atan2(lineobject->line().dx(),lineobject->line().dy());
}

QGraphicsLineItem* stick::lineObj(void){
    return lineobject;
}
//aggiorna la linea dello stick
void stick::setLine(QLineF* line)
{
    lineobject->setLine(*line);
    calcAngle();
}
//comando per cambiare la rotazione di uno stick
void stick::rotate(QPointF *point)
{
    //step 1 calcola l'angolo con alpha = atan2(dx/dy)
    int dx = static_cast<int>(lineobject->line().p1().x()-point->x());
    int dy = static_cast<int>(point->y()-lineobject->line().p1().y());
    float angle = atan2(dx,dy)*180/M_PI;
    //setta il pivot dello stick
    lineobject->setTransformOriginPoint(lineobject->line().p1());
    // lo stick punta al cursore che lo trascina
    lineobject->setRotation(angle+angleBuffer);
}

stick::~stick()
{
    delete lineobject;
}
