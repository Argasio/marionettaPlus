#include "stick.h"
#include <QGraphicsItem>

//lo stick si basa fondamentalmente su un QGraphicsLineObject, uno stick alloca una lineobject
stick::stick(QLineF *line)
{

    lineobject.setPen(myPen);
    lineobject.setFlag(QGraphicsItem::ItemIsSelectable,true);
    setLine(line);
}
//lo stick misura lángolo effettivo dell'oggetto linea rispetto al piano di disegno
void stick::calcAngle()
{
    angleBuffer = lineobject.line().angle();//(180/M_PI)*atan2(lineobject.line().dx(),lineobject.line().dy());
}

QGraphicsLineItem* stick::lineObj(void){
    return &lineobject;
}
//aggiorna la linea dello stick
void stick::setLine(QLineF* line)
{
    lineobject.setLine(*line);
    calcAngle();
}
//comando per cambiare la rotazione di uno stick
void stick::rotate(QPointF *point)
{
    //step 1 calcola l'angolo con alpha = atan2(dx/dy)
    int dx = static_cast<int>(lineobject.line().p1().x()-point->x());
    int dy = static_cast<int>(point->y()-lineobject.line().p1().y());
    float angle = atan2(dx,dy)*180/M_PI;
    //agisci sull'oggetto linea
    QLineF lBuf = lineobject.line();
    QPointF pBuf = getP2(&lBuf);
    //setta il pivot dello stick
    //lineobject.setTransformOriginPoint(lineobject.line().p1());
    qDebug("X %f, Y %f", lineobject.line().p2().x(),lineobject.line().p2().y());
    //progettiamo lóffset degli stick figlio
    QPointF oldEndPos = lineobject.line().p2();
    // lo stick punta al cursore che lo trascina
    lBuf.setAngle(-angle-90/*(angle+angleBuffer)*/);
    lineobject.setLine(lBuf);
    for(int i= 0; i<children.length();i++)
    {

        float DX = oldEndPos.x()-lineobject.line().p2().x();
        float DY = oldEndPos.y()-lineobject.line().p2().y();
        qDebug("DX %f, DY %f", DX,DY);
        qDebug("X2= %f, Y2= %f", pBuf.x(),pBuf.y());
        lBuf = children[i]->lineobject.line();
        lBuf.translate(-DX,-DY);
        children[i]->lineobject.setLine(lBuf);
    }
}
QPointF stick::getP2(QLineF* line)
{
    float x = line->p1().x()+line->length()*cos(line->angle());
    float y = line->p1().y()+line->length()*sin(line->angle());
    return QPointF(x,y);
}

stick::~stick()
{
}

