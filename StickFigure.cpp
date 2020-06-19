#include "StickFigure.h"


StickFigure::StickFigure()
{
    p0 = *new QPointF(0,0);
    p1 = *new QPointF(0,0);
    lineBuffer = *new QLineF(p0,p1);
    drawCount = 0;

}

//inizio disegno di una linea stick
QGraphicsLineItem* StickFigure::startDrawing(QPointF *point)
{
    if(stickList.isEmpty())
    {
        p0 = *point;
    }
    else
    {
        p0 = selectOrigin(point);
    }

    //crea e alloca l'oggetto stick associato
    stick *stk = new stick(&lineBuffer);
    stickBuffer = stk;
    drawCount = 1; // segnala che un disegno è in atto
    return stickBuffer->lineObj();
}
// aggiorna uno stick il cui disegno è già avviato
QGraphicsLineItem *StickFigure::previewDrawing(QPointF *point)
{
    p1 = *point;
    lineBuffer = QLineF(p0,p1);
    // lo stick si fonda sulla linea che connette starting point e endpoint
    stickBuffer->setLine(&lineBuffer);
    return stickBuffer->lineObj();
}

QGraphicsLineItem *StickFigure::endDrawing(QPointF *point)
{
    previewDrawing(point);          //aggiorna il disegno
    drawCount = 0;                  //segnala la fine
    stickList.append(stickBuffer);  //disegno finito, aggiungilo alla lista
    itemList.append(stickBuffer->lineObj());
    return stickBuffer->lineObj();
}

QGraphicsLineItem *StickFigure::returnLine()
{
    return stickBuffer->lineObj();
}

void StickFigure::cancelDrawing()
{
    delete[] stickBuffer;
    drawCount = 0;
}

//scegli come origine lo stick più vicino al mouse fra gli stick dello stickfigure
QPointF StickFigure::selectOrigin( QPointF * point)
{
    int idx         = 0;
    float  minDist  = 0;
    parentBuffer = -1;
    QPointF pBuf1,pBuf2,pBufOut;
    //inizializziamo a partire dall'origine del primo stick;
    pBufOut = qgraphicsitem_cast<QGraphicsLineItem*>(stickList[idx]->lineObj())->line().p1();
    minDist = QLineF(pBufOut,*point).length();
    while(idx< itemList.length())
    {
        // i punti inizio-fine dello stick corrente
        pBuf1 = qgraphicsitem_cast<QGraphicsLineItem*>(stickList[idx]->lineObj())->line().p1();
        pBuf2 = qgraphicsitem_cast<QGraphicsLineItem*>(stickList[idx]->lineObj())->line().p2();
        // se la distanza è più bassa di quella minima registrata dal luogo del mouse
        if(QLineF(pBuf2,*point).length()<= minDist)
        {
            // quello è l'origine
            pBufOut = pBuf2;
            minDist = QLineF(pBuf2,*point).length();
            parentBuffer = idx;

        }
    }
    return pBufOut;
}
