#include "stick.h"
#include <QGraphicsItem>

//lo stick si basa fondamentalmente su un QGraphicsLineObject, uno stick alloca una lineobject
stick::stick(QLineF *line)
{
    this->setFlag(QGraphicsItem::ItemIsSelectable,true);
    QPen Pen = QPen();
    highlight = true;
    setLine(line);
}
void stick::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    //Pen.setColor(Qt::black);
    this->setZValue(Z);
    painter->setPen(Pen);
    painter->drawLine(myLine.x1(),myLine.y1(),myLine.x2(),myLine.y2());
    //Pen.setColor(Qt::red); //draw bounding box
    if(this->highlight)
        painter->setPen(QPen(Qt::green,10));
    else
        painter->setPen(QPen(Qt::red,10));
    painter->drawEllipse(myLine.p2().x()-5,myLine.p2().y()-5,10,10);
    //painter->drawRect(br);
}
// bounding rectangle deve essere sempre "positivo"
QRectF stick::boundingRect() const
{
    return this->br;
}
//funzione per aggiornare il boudning rectangle
QRectF stick::updateBr(int mode)
{
    QRectF newBr;
    qreal oldP1x;
    qreal oldP1y;
    qreal oldP2x;
    qreal oldP2y;
    qreal newP1x;
    qreal newP1y;
    qreal newP2x;
    qreal newP2y;
    qreal fP1x;
    qreal fP1y;
    qreal fP2x;
    qreal fP2y;
    // 1 Assicurati che a seconda della direzione della linea, il rettangolo abbia come P1 il punto più in alto a sx
    //      e come P2 il punto più in basso a destra
        if(myLine.dy()>0 && myLine.dx()>0)
            newBr = QRectF(myLine.p1(),myLine.p2());
        else if(myLine.dy()>0 && myLine.dx()<=0)
            newBr = QRectF(QPointF(myLine.p2().x(),myLine.p1().y()),
                            QPointF(myLine.p1().x(),myLine.p2().y()));
        else if(myLine.dy()<=0 && myLine.dx()>0)
            newBr = QRectF(QPointF(myLine.p1().x(),myLine.p2().y()),
                            QPointF(myLine.p2().x(),myLine.p1().y()));
        else
            newBr = QRectF(myLine.p2(),myLine.p1());
        newBr.setX(newBr.x()-Pen.width());newBr.setY(newBr.y()-Pen.width()); // TODO padding proporzionale alla dimensione della Penna
        newBr.setWidth(newBr.width()+Pen.width());
        newBr.setHeight(newBr.height()+Pen.width());
        this->br.getCoords(&oldP1x,&oldP1y,&oldP2x,&oldP2y);
        // Se stiamo muovendo l'oggetto, teniamo conto anche del precedente bounding rect, e li fondiamo
        // per creare un bounding rect che contenga sia quello nuovo che quello vecchio così da non generare ghosting
        if(mode == 1){
            newBr.getCoords(&newP1x,&newP1y,&newP2x,&newP2y);
            if(oldP1x<=newP1x)
                fP1x = oldP1x;
            else
                fP1x = newP1x;
            if(oldP1y<=newP1y)
                fP1y = oldP1y;
            else
                fP1y = newP1y;
            if(oldP2x>=newP2x)
                fP2x = oldP2x;
            else
                fP2x = newP2x;
            if(oldP2y>=newP2y)
                fP2y = oldP2y;
            else
                fP2y = newP2y;
            newBr = QRectF(QPointF(fP1x,fP1y),QPointF(fP2x,fP2y));
        }
        return newBr;

}
//lo stick misura l'angolo effettivo dell'oggetto linea rispetto al piano di disegno
void stick::calcAngle()
{
    angleBuffer = myLine.angle();//(180/M_PI)*atan2(lineobject.line().dx(),lineobject.line().dy());
}

//aggiorna la linea dello stick
void stick::setLine(QLineF* line)
{
    //lineobject.setLine(*line);
    myLine = *line;
}
void stick::refresh(int mode)
{
    prepareGeometryChange();    // necessario chiamare questa funzione per non lasciare trails ed evitare il ghosting durante la manipolazione e il disegno!
    br = updateBr(mode);
    calcAngle();
    update();
}
//comando per cambiare la rotazione di uno stick
void stick::rotate(QPointF *point)
{
    //step 1 calcola l'angolo con alpha = atan2(dx/dy)
    int dx = static_cast<int>(myLine.p1().x()-point->x());
    int dy = static_cast<int>(-myLine.p1().y()+point->y());
    float angle         = atan2(dx,dy)*180/M_PI;
    //agisci sull'oggetto linea
    QPointF pBuf        = getP2(&myLine);
    //setta il pivot dello stick
    //lineobject.setTransformOriginPoint(lineobject.line().p1());
    //qDebug("X %f, Y %f", lineobject.line().p2().x(),lineobject.line().p2().y());
    //progettiamo lóffset degli stick figlio
    QPointF oldEndPos   = myLine.p2();
    // lo stick punta al cursore che lo trascina
    myLine.setAngle(-angle-90/*(angle+angleBuffer)*/);
    refresh(1);
    for(int i= 0; i<children.length();i++)
    {
        float DX = oldEndPos.x()-myLine.p2().x();
        float DY = oldEndPos.y()-myLine.p2().y();
        qDebug("DX %f, DY %f", DX,DY);
        qDebug("X2= %f, Y2= %f", pBuf.x(),pBuf.y());
        children[i]->myLine.translate(-DX,-DY);
        children[i]->refresh(1);
    }

}
void stick::endRotation()
{
    refresh(0);
    for(int i= 0; i<children.length();i++)
    {
        children[i]->refresh(0);
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

