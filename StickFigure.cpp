#include "StickFigure.h"
// StickFigure work in the one active scene
extern QGraphicsScene * scene;

StickFigure::StickFigure()
{
    p0 =  QPointF(0,0);
    p1 = QPointF(0,0);
    lineBuffer =  QLineF(p0,p1);
    drawCount = 0;

}

//inizio disegno di una linea stick
QGraphicsLineItem* StickFigure::startDrawing(QPointF *point)
{
    if(stickList.isEmpty()) // se è il primo stick dello stickfigure
    {
        p0 = *point;
    }
    else //se ci sono altri stick in questo stickfigure i dalla estremità più vicinaz al click
    {
        p0 = selectOrigin(point);
    }

    //crea e alloca l'oggetto stick associato
    lineBuffer = QLineF(p0,p0); // all'inizio la linea ha lunghezza zero
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
    stickBuffer->parent = parentBuffer;
    // INSERISCILO COME FIGLIO DI TUTTA LA GERARCHIA PRECEDENTE
    while(parentBuffer){
        parentBuffer->children.append(stickBuffer);
        parentBuffer = parentBuffer->parent;
    }


    return stickBuffer->lineObj();
}

QGraphicsLineItem *StickFigure::returnLine()
{
    return stickBuffer->lineObj();
}

void StickFigure::cancelDrawing()
{
    delete stickBuffer;
    lineBuffer = QLineF();
    drawCount = 0;
}

//scegli come origine lo stick più vicino al mouse fra gli stick dello stickfigure
QPointF StickFigure::selectOrigin( QPointF * point)
{
    int idx         = 0;
    float  minDist  = 0;
    parentBuffer    = NULL;
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
            parentBuffer = stickList[idx];
        }
        idx++;
    }
    return pBufOut;
}

void StickFigure::deleteStick(int idx)
{
    stick *selectedStick = stickList[idx];   //oggetto stick che contiene l'oggetto grafico
    scene->removeItem(itemList[idx]); //rimuovi l'oggetto dalla scena
    //per ogni figlio dell'oggetto, rimuovi gli oggetti dalla scena e cancellali
    for(stick * s : selectedStick->children){
        int childIndex = itemList.indexOf(s->lineObj());
        stick *childStick = stickList[childIndex];   //oggetto stick che contiene l'oggetto grafico
        scene->removeItem(itemList[childIndex]); //rimuovi l'oggetto dalla scena
        itemList.removeAt(childIndex);                  //rimuovi dalla lista degli stick e dalla lista  degli oggetti grafici
        stickList.removeAt(childIndex);
        stick * parent = selectedStick->parent;
        // rimuovi a ritroso ciascuno stick figlio di quello selezionato dalle listte figli di tutti gli altri sitck presenti genitori di esso
        while(parent != NULL )
        {
            int removefromparentIdx = parent->children.indexOf(childStick);
            parent->children.removeAt(removefromparentIdx);
            parent = parent->parent;
        }
        delete childStick;
    }
    // rimuovi lo stick selezionato dalla lista dei suoi genitori
    itemList.removeAt(idx);                  //rimuovi dalla lista degli stick e dalla lista  degli oggetti grafici
    stickList.removeAt(idx);
    stick * parent = selectedStick->parent;
    while(parent != NULL)
    {
        int removefromparentIdx = parent->children.indexOf(selectedStick);
        parent->children.removeAt(removefromparentIdx);
        parent = parent->parent;
    }
    delete selectedStick;
    lineBuffer = QLineF();

}


