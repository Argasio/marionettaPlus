#include "myview.h"
#include <math.h>
#include <QDebug>
#include <QGraphicsRotation>
#include <frame.h>
bool    isPressed = false;
QPointF startingCoord; //mouse click coordinateBuffer
QPointF coord; //current mouse pos
float   rotationBuffer; //selected item Rotation buffer
QPen    myPen;
QBrush  myBrush;

Animation * myAnimation;
myView::myView(QWidget *parent) : QGraphicsView(parent)
{
    myAnimation = new Animation();

    setMouseTracking(true);
    myPen.setWidth(10);
}

void myView::setTool(int Tool)
{
    changeTool();
    tool = Tool;
}

void myView::mousePressEvent(QMouseEvent *event)
{
    isPressed       = true; //segnala che è premuto
    coord           = mapToScene(event->pos());// converti le coordinate del mouse in coordinate di scena
    startingCoord   = coord; //buffer per il dragghing del mouse premuto
    switch(tool)
    {
        case NOTOOL:
        {
            if(scene()->selectedItems().count() == 1)
            {
            }
            break;
        }
        case DRAW:
        {
            drawCmd(&coord);
            break;
        }
    }
    QGraphicsView::mousePressEvent(event);
}

void myView::keyPressEvent(QKeyEvent *event)
{
    switch(event->key())
    {
        case(Qt::Key_Cancel):
        case(Qt::Key_Delete):
        {
            if(scene()->selectedItems().count() == 1)
            {
                QGraphicsItem *item = scene()->selectedItems()[0];//oggetto selezionato nella scnea
                int idx = myAnimation->frameBuffer->stickFigureBuffer->itemList.indexOf(item);        // indice corrispondente nella lista ordinata degli oggetti
                //stick *selectedStick = myAnimation->frameBuffer->stickFigureBuffer->stickList[idx];
                //selectedStick->deleteStick(QGraphicsScene* scene);
                myAnimation->frameBuffer->stickFigureBuffer->deleteStick( idx);
                // distruggi l'oggetto
                qDebug("item deleted");
            }
            break;
        }
        case(Qt::Key_Escape):
        {
            switch(tool)
            {
                case(DRAW):
                {
                    if(myAnimation->frameBuffer->stickFigureBuffer->drawCount==1)
                    {
                        scene()->removeItem(myAnimation->frameBuffer->stickFigureBuffer->returnLine());
                        myAnimation->frameBuffer->stickFigureBuffer->cancelDrawing();
                    }
                    break;
                }
            }
            break;
        }
    }

    QGraphicsView::keyPressEvent(event);
}

// il disegno di una linea conta 2 click
// il primo determina lo starting point
// il secondo l'end point
void myView::drawCmd(QPointF* point)
{
    if(myAnimation->frameBuffer->stickFigureBuffer->drawCount == 0){
        //aggiungi il nuovo elemento alla scena mediante puntatore dell'elemento linea dell'oggetto stick
       QGraphicsLineItem * item =  myAnimation->frameBuffer->stickFigureBuffer->startDrawing(point);
       scene()->addItem(qgraphicsitem_cast<QGraphicsLineItem*>(item));
       qDebug("Draw 1 = %f, %f",point->x(),point->y());
    }
    else if(myAnimation->frameBuffer->stickFigureBuffer->drawCount >0)
    {
        myAnimation->frameBuffer->stickFigureBuffer->endDrawing(point);
        qDebug("Draw 2 = %f, %f",point->x(),point->y());
    }
}
//Esci dallo strumento che stai usando
void myView::changeTool()
{
    switch(tool)
    {
        case(NOTOOL):
        {
            break;
        }
        case(DRAW):
        {
            // se stavi disegnando uno stick e cambi prima di averlo finito, cancellalo!
            if(myAnimation->frameBuffer->stickFigureBuffer->drawCount==1)
            {
                scene()->removeItem(myAnimation->frameBuffer->stickFigureBuffer->returnLine());
                myAnimation->frameBuffer->stickFigureBuffer->cancelDrawing();
            }
            break;
        }
    }
}

// mouse rilasciato
void myView::mouseReleaseEvent(QMouseEvent *event)
{
    isPressed = false;
    QGraphicsView::mouseReleaseEvent(event);
}

// Se il mouse viene mosso durante il disegno di una linea continua ad aggiornarne il disegno
// Se viene mosso durante una pressione con stick selezionato, agisci sul relativo puntatore all'elemento linea
// per ruotarla seguendo il mouse
void myView::mouseMoveEvent(QMouseEvent *event)
{
    coord = mapToScene(event->pos());
    switch(tool)
    {
        case NOTOOL:
        {
            if(isPressed)
            {
                if(scene()->selectedItems().count() == 1)
                {
                    //trova l'indice dello stick selezionato dentro la lista nella scena cercando il puntatore nelle liste
                    QGraphicsItem* item = scene()->selectedItems()[0];
                    myAnimation->updateSelection((QGraphicsLineItem*)item);
                    //qDebug("selected item %d", idx);
                    // Ora ruota lo stick
                    myAnimation->currentFrame->currentStickFigure->currentStick->rotate(&coord);
                }
            }
            break;
        }
        case DRAW:
        {
        // durante il disegno,l'estremo libero della linea segue il mouse
            if(myAnimation->frameBuffer->stickFigureBuffer->drawCount > 0)
            {
                myAnimation->frameBuffer->stickFigureBuffer->previewDrawing(&coord);
                qDebug("coord = %f, %f",coord.x(),coord.y());
            }
            break;
        }
    }
    QGraphicsView::mouseMoveEvent(event);
}


