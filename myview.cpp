#include "myview.h"
#include <math.h>
#include <QDebug>
#include <QGraphicsRotation>
#include <frame.h>
#include <QMessageBox>
bool    isPressed = false;
QPointF startingCoord; //mouse click coordinateBuffer
QPointF coord; //current mouse pos
float   rotationBuffer; //selected item Rotation buffer
QPen    myPen;
QBrush  myBrush;
extern QListWidget * myStickFigureWidgetList;

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
StickFigure * myView::addStickFigure(QListWidgetItem* item)
{
    StickFigure *S = myAnimation->currentFrame->addStickFigure(item);
    S->scene = this->scene();

}

void myView::deleteStickFigure()
{
    if(myStickFigureWidgetList->selectedItems().count() == 1 /*&& myAnimation->currentFrame->stickFigures.count()>1*/)
    {
        QListWidgetItem *current = myStickFigureWidgetList->selectedItems()[0];
        QVariant  retrievedData = (current->data(Qt::UserRole));
        StickFigure* cs = qvariant_cast<StickFigure*>(retrievedData);
        StickFigure* ns = myAnimation->currentFrame->removeStickFigure(cs);
        myStickFigureWidgetList->takeItem(myStickFigureWidgetList->row(current));
        delete current;
        if(ns!=NULL)
        {
            myAnimation->updateSelection(ns);
        }
    }
    else{

    }
}

void myView::moveStickFigureZ(int increment)
{
    if(myStickFigureWidgetList->count()>0)
    {
        myAnimation->currentFrame->currentStickFigure->baseZ+increment;
        myStickFigureWidgetList->clearSelection();
        int currentIndex = myStickFigureWidgetList->currentRow();
        if(!(increment>0 && currentIndex == myStickFigureWidgetList->count()-1) && !(increment<0 && currentIndex == 0))
        {
            QListWidgetItem * item = myStickFigureWidgetList->takeItem(currentIndex);
            myStickFigureWidgetList->insertItem(currentIndex+increment,item);
            myStickFigureWidgetList->setItemSelected(item,true);

            //myStickFigureWidgetList->setItemSelected(myAnimation->currentFrame->currentStickFigure->linkedItem,true);
        }
    }

}
void myView::mousePressEvent(QMouseEvent *event)
{
    isPressed       = true; //segnala che è premuto
    coord           = mapToScene(event->pos());// converti le coordinate del mouse in coordinate di scena
    startingCoord   = coord; //buffer per il dragghing del mouse premuto
    switch(tool)
    {
        if(!myAnimation->currentFrame->stickFigures.isEmpty())
        {
            case NOTOOL:
            {
                myAnimation->updateSelection(coord);
                myStickFigureWidgetList->clearSelection();
                myStickFigureWidgetList->setItemSelected(myAnimation->currentFrame->currentStickFigure->linkedItem,true);
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
                stick *item = (stick*)scene()->selectedItems()[0];//oggetto selezionato nella scnea
                int idx = myAnimation->frameBuffer->currentStickFigure->stickList.indexOf(item);        // indice corrispondente nella lista ordinata degli oggetti
                if(myAnimation->frameBuffer->currentStickFigure->stickList[idx]->master)
                {
                    QListWidgetItem * associatedItem = myAnimation->frameBuffer->currentStickFigure->linkedItem;
                    myStickFigureWidgetList->clearSelection();
                    myStickFigureWidgetList->setItemSelected(associatedItem,true);
                    deleteStickFigure();
                }
                else
                    myAnimation->frameBuffer->currentStickFigure->deleteStick( idx);
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
                    if(myAnimation->frameBuffer->currentStickFigure->drawCount==1)
                    {
                        scene()->removeItem((QGraphicsItem*)myAnimation->frameBuffer->currentStickFigure->currentStick);
                        myAnimation->frameBuffer->currentStickFigure->cancelDrawing();
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
    if(!myAnimation->currentFrame->stickFigures.isEmpty())
    {
        if(myAnimation->currentFrame->currentStickFigure->drawCount == 0){
            myAnimation->currentFrame->currentStickFigure->startDrawing(point);
            myAnimation->currentFrame->currentStickFigure->currentStick->Pen = myPen;
            //aggiungi il nuovo elemento alla scena mediante puntatore dell'elemento linea dell'oggetto stick
           scene()->addItem(myAnimation->currentFrame->currentStickFigure->currentStick);
           qDebug("Draw 1 = %f, %f",point->x(),point->y());
        }
        else if(myAnimation->frameBuffer->currentStickFigure->drawCount >0)
        {
            myAnimation->currentFrame->currentStickFigure->endDrawing(point);
            qDebug("Draw 2 = %f, %f",point->x(),point->y());
        }
    }
    else
    {
        QMessageBox::about(this,"error","no stickfigure layer present, please add a stickfigure");
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
            if(!myAnimation->currentFrame->stickFigures.isEmpty())
            {
                // se stavi disegnando uno stick e cambi prima di averlo finito, cancellalo!
                if(myAnimation->frameBuffer->currentStickFigure->drawCount==1)
                {
                    scene()->removeItem((QGraphicsItem*)myAnimation->frameBuffer->currentStickFigure->currentStick);
                    myAnimation->frameBuffer->currentStickFigure->cancelDrawing();
                }
            }
            else{

            }
            break;
        }
    }
}

// mouse rilasciato
void myView::mouseReleaseEvent(QMouseEvent *event)
{
    isPressed = false;
    if(myAnimation->currentFrame->currentStickFigure != NULL)
    {
        myAnimation->currentFrame->currentStickFigure->currentStick->endRotation();
    }
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
                    stick* item = (stick*)scene()->selectedItems()[0];
                    myAnimation->updateSelection(item);

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
            if(myAnimation->currentFrame->currentStickFigure != NULL)
            {
                if(myAnimation->frameBuffer->currentStickFigure->drawCount > 0)
                {
                    myAnimation->frameBuffer->currentStickFigure->previewDrawing(&coord);
                    qDebug("coord = %f, %f",coord.x(),coord.y());
                }
            }
            else
            {
            }
            break;
        }
    }
    QGraphicsView::mouseMoveEvent(event);
}


