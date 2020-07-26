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
    myAnimation->scene = scene();
    setMouseTracking(true);
    myPen.setWidth(10);
    myPen.setCapStyle(Qt::RoundCap);
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
    return S;
}

void myView::deleteStickFigure()
{
    if(myStickFigureWidgetList->selectedItems().count() == 1 && myAnimation->currentFrame->stickFigures.count()>1)
    {
        //scegli lo stickfigure selzionato dalla lista grafica
        QListWidgetItem *current = myStickFigureWidgetList->selectedItems()[0];
        // estrai il dato utente salvato in esso, puntatore ad uno stickfigure
        QVariant  retrievedData = (current->data(Qt::UserRole));
        //riconverti il dato
        StickFigure* cs = qvariant_cast<StickFigure*>(retrievedData);
        
        //distruggi l'oggetto della lista
        myStickFigureWidgetList->takeItem(myStickFigureWidgetList->row(current));
        delete current;
        //rimuovilo dal frame e distruggilo
        StickFigure* ns = myAnimation->currentFrame->removeStickFigure(cs);
        // Ora controlliamo la nuova selezione non sia nulla e aggiorniamo la selezione
        // dell'elenco del widget
        if(ns!=NULL)
        {
            myAnimation->updateSelection(ns);
            myStickFigureWidgetList->clearSelection();
            myStickFigureWidgetList->setItemSelected(ns->linkedItem,true);
        }
    }
    else{

    }

}
//cambia l'ordine di profondità dello stickfigure selezionato
void myView::moveStickFigureZ(int increment)
{
    if(myStickFigureWidgetList->count()>0)
    {
        myStickFigureWidgetList->clearSelection();
        int currentIndex = myStickFigureWidgetList->currentRow();
        if(!(currentIndex+1 == myStickFigureWidgetList->count() && increment>0)||
                !(currentIndex == 0 && increment<0))
        {
            myAnimation->currentFrame->currentStickFigure->baseZ+=increment;
            myAnimation->currentFrame->currentStickFigure->updateZ();
            if(!(increment>0 && currentIndex == myStickFigureWidgetList->count()-1) && !(increment<0 && currentIndex == 0))
            {
                //sposta nella lista grafica il layer dello stickfigure
                QListWidgetItem * item = myStickFigureWidgetList->takeItem(currentIndex);
                myStickFigureWidgetList->insertItem(currentIndex+increment,item);
                myStickFigureWidgetList->setItemSelected(item,true);
            }
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
            case DRAWCIRCLE:
            {
                drawCmd(&coord,(int)tool);
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
                // se si tratta dello stick master e ci sono altri stickfigure, cancella l'intero stickfigure
                if(myAnimation->frameBuffer->currentStickFigure->stickList[idx]->master &&
                        myAnimation->frameBuffer->stickFigures.count()>1)
                {
                    //estrai dalla lista grafica l'elemento selezionato collegato allo stickfigure
                    QListWidgetItem * associatedItem = myAnimation->frameBuffer->currentStickFigure->linkedItem;
                    myStickFigureWidgetList->clearSelection();
                    myStickFigureWidgetList->setItemSelected(associatedItem,true);
                    //cancella lo stickfigure
                    deleteStickFigure();
                }
                //altrimenti cancella il singolo stick e aggiorna l'iconea del livello
                else
                {
                    myAnimation->frameBuffer->currentStickFigure->deleteStick( idx);
                    myAnimation->currentFrame->currentStickFigure->updateIcon();
                    myStickFigureWidgetList->selectedItems()[0]->setIcon(*myAnimation->currentFrame->currentStickFigure->stickFigureIcon);
                }
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
                case DRAWCIRCLE:
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
void myView::drawCmd(QPointF* point, int mode)
{
    if(!myAnimation->currentFrame->stickFigures.isEmpty())
    {
        if(myAnimation->currentFrame->currentStickFigure->drawCount == 0){
            myAnimation->currentFrame->currentStickFigure->startDrawing(point);
            // se stiamo usando il tool per generare cerchi cambiamo il tipo dello stick
            if(mode == DRAWCIRCLE){
                myAnimation->currentFrame->currentStickFigure->currentStick->type = stick::stickType::CIRCLE;
            }

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
        case(DRAWCIRCLE):
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
    if(myAnimation->currentFrame->stickFigures.count()>0)
    {
        if(myAnimation->currentFrame->currentStickFigure->stickList.count()>0)
        {
            //termina una rotazione di stick al rilascio
            myAnimation->currentFrame->currentStickFigure->currentStick->endRotation();
            myAnimation->currentFrame->currentStickFigure->updateIcon();
            myStickFigureWidgetList->selectedItems()[0]->setIcon(*myAnimation->currentFrame->currentStickFigure->stickFigureIcon);

        }
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
                    if(myAnimation->currentFrame->currentStickFigure->stickList.count()>0)

                        myAnimation->currentFrame->currentStickFigure->currentStick->rotate(&coord);
                }
            }
            break;
        }
        case DRAW:
        case DRAWCIRCLE:
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


