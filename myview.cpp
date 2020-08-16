#include "myview.h"
#include <math.h>
#include <QDebug>
#include <QGraphicsRotation>
#include <frame.h>
#include <QMessageBox>
#include <QSpinBox>
#include <QBuffer>
bool    isPressed = false;
QPointF startingCoord; //mouse click coordinateBuffer
QPointF coord; //current mouse pos
float   rotationBuffer; //selected item Rotation buffer
bool clearUndoFlag = false;
extern QListWidget * myStickFigureWidgetList;
extern QListWidget * myFrameWidgetList;
bool undoFlag = false;
extern QSpinBox * onionSkinSB;

extern int W;
extern int H;
extern QGraphicsRectItem* myRect;
extern bool playBack ;
extern bool loadFile;
extern QPointF onionOffset;

myView::myView(QWidget *parent) : QGraphicsView(parent)
{
    onionRender = false;
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
/*
StickFigure * myView::addStickFigure(QListWidgetItem* item)
{
    StickFigure *S = myAnimation->currentFrame->addStickFigure(item);
    S->scene = this->scene();
    return S;
}*/

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
        if(ns!=NULL && ns!=nullptr)
        {
            myAnimation->updateSelection(ns);
            myStickFigureWidgetList->clearSelection();
            myStickFigureWidgetList->setItemSelected(ns->linkedItem,true);
        }
        myAnimation->currentFrame->updateIcon();
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
                myStickFigureWidgetList->setCurrentRow(currentIndex+increment);
            }
            myAnimation->currentFrame->updateIcon();
        }
    }
}
void myView::mapMyCoords(QPointF& myCoord, QPointF startingPoint){
    coord = mapToScene(startingPoint.x(),startingPoint.y());
    if(myCoord.x()<-myRect->rect().width()/10)
        myCoord.setX(-myRect->rect().width()/10);
    if(myCoord.y()<-myRect->rect().height()/10)
        myCoord.setY(-myRect->rect().height()/10);
    if(myCoord.y()>myRect->rect().height()+myRect->rect().height()/10)
        myCoord.setY(myRect->rect().height()+myRect->rect().height()/10);
    if(myCoord.x()>myRect->rect().width()+myRect->rect().width()/10)
        myCoord.setX(myRect->rect().width()+myRect->rect().width()/10);
}
void myView::mousePressEvent(QMouseEvent *event)
{
    isPressed       = true; //segnala che è premuto
    // converti le coordinate del mouse in coordinate di scena
    mapMyCoords(coord,event->pos());


    startingCoord   = coord; //buffer per il dragghing del mouse premuto
    switch(tool)
    {
        if(!myAnimation->currentFrame->stickFigures.isEmpty())
        {
            case NOTOOL:
            {
                storeUndo();
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
                storeUndo();
                stick *item = (stick*)scene()->selectedItems()[0];//oggetto selezionato nella scnea
                int idx = myAnimation->currentFrame->currentStickFigure->stickList.indexOf(item);        // indice corrispondente nella lista ordinata degli oggetti
                // se si tratta dello stick master e ci sono altri stickfigure, cancella l'intero stickfigure
                if(myAnimation->currentFrame->currentStickFigure->stickList[idx]->master &&
                        myAnimation->currentFrame->stickFigures.count()>1)
                {
                    //estrai dalla lista grafica l'elemento selezionato collegato allo stickfigure
                    QListWidgetItem * associatedItem = myAnimation->currentFrame->currentStickFigure->linkedItem;
                    myStickFigureWidgetList->clearSelection();
                    myStickFigureWidgetList->setItemSelected(associatedItem,true);
                    //cancella lo stickfigure
                    deleteStickFigure();
                }
                //altrimenti cancella il singolo stick e aggiorna l'iconea del livello
                else if(myStickFigureWidgetList->selectedItems().count()==1)
                {
                    myAnimation->currentFrame->currentStickFigure->deleteStick( idx);
                    myAnimation->currentFrame->currentStickFigure->updateIcon();
                    myStickFigureWidgetList->selectedItems()[0]->setIcon(*myAnimation->currentFrame->currentStickFigure->stickFigureIcon);
                }
                myAnimation->currentFrame->updateIcon();
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
                    if(myAnimation->currentFrame->currentStickFigure->drawCount==1)
                    {
                        scene()->removeItem((QGraphicsItem*)myAnimation->currentFrame->currentStickFigure->currentStick);
                        myAnimation->currentFrame->currentStickFigure->cancelDrawing();
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
            storeUndo();
            myAnimation->currentFrame->currentStickFigure->startDrawing(point, myPen);
            // se stiamo usando il tool per generare cerchi cambiamo il tipo dello stick
            if(mode == DRAWCIRCLE){
                myAnimation->currentFrame->currentStickFigure->currentStick->type = stick::stickType::CIRCLE;
            }

            myAnimation->currentFrame->currentStickFigure->currentStick->Pen = myPen;
            //aggiungi il nuovo elemento alla scena mediante puntatore dell'elemento linea dell'oggetto stick
           scene()->addItem(myAnimation->currentFrame->currentStickFigure->currentStick);
           qDebug("Draw 1 = %f, %f",point->x(),point->y());
        }
        else if(myAnimation->currentFrame->currentStickFigure->drawCount >0)
        {
            myAnimation->currentFrame->currentStickFigure->endDrawing(point);
            qDebug("Draw 2 = %f, %f",point->x(),point->y());
            myAnimation->currentFrame->updateIcon();
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
                if(myAnimation->currentFrame->currentStickFigure->drawCount==1)
                {
                    scene()->removeItem((QGraphicsItem*)myAnimation->currentFrame->currentStickFigure->currentStick);
                    myAnimation->currentFrame->currentStickFigure->cancelDrawing();
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
        if(myAnimation->currentFrame->currentStickFigure->stickList.count()>0
                 && myStickFigureWidgetList->selectedItems().count() == 1)
        {
            //termina una rotazione di stick al rilascio
            myAnimation->currentFrame->currentStickFigure->currentStick->endRotation();
            myAnimation->currentFrame->currentStickFigure->updateIcon();
            myStickFigureWidgetList->selectedItems()[0]->setIcon(*myAnimation->currentFrame->currentStickFigure->stickFigureIcon);
            myAnimation->currentFrame->updateIcon();
        }
    }
    QGraphicsView::mouseReleaseEvent(event);
}

// Se il mouse viene mosso durante il disegno di una linea continua ad aggiornarne il disegno
// Se viene mosso durante una pressione con stick selezionato, agisci sul relativo puntatore all'elemento linea
// per ruotarla seguendo il mouse
void myView::mouseMoveEvent(QMouseEvent *event)
{

    mapMyCoords(coord, event->pos());
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
            if(myAnimation->currentFrame->currentStickFigure != NULL && myAnimation->currentFrame->currentStickFigure != nullptr)
            {
                if(myAnimation->currentFrame->currentStickFigure->drawCount > 0)
                {
                    myAnimation->currentFrame->currentStickFigure->previewDrawing(&coord);
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
Frame* myView::setUpFrame(int pos){
    //decidi il nome
    //int intName = 0;
    /*if(myAnimation->frameList.count()>=1){
        intName = myAnimation->currentFrame->frameNumber+1;

    }*/
    return myAnimation->setupFrame(pos);
}
void myView::deleteFrame(Frame* frame){
    int frameNum = frame->frameNumber;
    myFrameWidgetList->takeItem(myFrameWidgetList->row(frame->linkedItem));
    myAnimation->deleteFrame(frame);
    myFrameWidgetList->clearSelection();
    for(Frame *f: myAnimation->frameList){
        if(f->frameNumber>frameNum){
            f->frameNumber--;
            f->linkedItem->setData(Qt::DisplayRole, QString::number(f->frameNumber));
        }
    }
    myFrameWidgetList->setItemSelected(myAnimation->currentFrame->linkedItem,true);
}
void myView::moveToFrame(Frame* frame){
    //myAnimation->curreDrakasiontFrame->updateRender();
    QRectF myR = scene()->sceneRect();
    int r = 0;
    if(myAnimation->frameList.count()>=1){
        for(StickFigure *S:myAnimation->currentFrame->stickFigures){
            for(stick * s: S->stickList){

               scene()->removeItem(s);
            }
        }
    }
    myStickFigureWidgetList->clear();
    for(StickFigure *S:frame->stickFigures){
        QListWidgetItem * addedItem = new QListWidgetItem(myStickFigureWidgetList);
        QVariant newData(QVariant::fromValue(S));
        addedItem->setData(Qt::UserRole,newData);
        addedItem->setData(Qt::DisplayRole,S->name);
        addedItem->setIcon(*S->stickFigureIcon);
        S->linkedItem = addedItem;
        myStickFigureWidgetList->addItem(S->linkedItem);
        for(stick * s: S->stickList){
            scene()->addItem(s);

        }

    }

    myAnimation->currentFrame = frame;
    if(!myAnimation->currentFrame->stickFigures.isEmpty())
        myAnimation->currentFrame->currentStickFigure = myAnimation->currentFrame->stickFigures[0];
    myStickFigureWidgetList->clearSelection();
    myFrameWidgetList->clearSelection();
    scene()->clearSelection();
    if(!myAnimation->currentFrame->stickFigures.isEmpty())
        if(!myAnimation->currentFrame->stickFigures[0]->stickList.isEmpty())
            myAnimation->updateSelection(myAnimation->currentFrame->stickFigures[0]->stickList[0]);
    updateOnionSkins();

    scene()->update(myR);
    if(playBack == false){
        myFrameWidgetList->setItemSelected(myAnimation->currentFrame->linkedItem,true);
        myFrameWidgetList->setCurrentItem(myAnimation->currentFrame->linkedItem);
        if(!myAnimation->frameList.isEmpty() && !myAnimation->currentFrame->stickFigures.isEmpty()){
            myAnimation->currentFrame->currentStickFigure->updateIcon();
            myFrameWidgetList->selectedItems()[0]->setIcon(*myAnimation->currentFrame->frameIcon);
            myAnimation->currentFrame->updateIcon();
        }
        if(myRect == nullptr){
            myRect = new QGraphicsRectItem(0,0,W,H);
        }
        scene()->addItem(myRect);
    }

}
void myView::updateOnionSkins(){

    for(QGraphicsPixmapItem* P:onionSkins){
        delete P;
    }
    onionSkins.clear();
    if(onionSkinSB->value()==0 || myAnimation->currentFrame->frameNumber == 0)
        return;
    //
    onionRender = true;
    QGraphicsScene renderScene;
    QImage *renderImg = new QImage(scene()->sceneRect().width(),scene()->sceneRect().height(),QImage::Format_ARGB32);

    renderScene.setSceneRect(scene()->sceneRect());
    // ora clona tutti gli stick nello stickfigure in una tempList, aggiungili alla scena fittizia
    QList<stick*> tempList;


    //



    int skinNum = onionSkinSB->value();
    float opacityVal = 0;
    if(myAnimation->currentFrame->frameNumber<skinNum)
        skinNum = myAnimation->currentFrame->frameNumber;

    for(int i = myAnimation->currentFrame->frameNumber-skinNum; i<myAnimation->currentFrame->frameNumber; i++){

        for(StickFigure *S:myAnimation->frameList[i]->stickFigures)
        {
            for(stick* s: S->stickList){
                stick* clone = new stick(s);
                tempList.append(clone);
                renderScene.addItem(clone);
            }
        }
        QPainter painter(renderImg);
        renderImg->fill(Qt::transparent);
        painter.setBackground(QBrush(QColor(Qt::transparent)));
        renderScene.render(&painter);


        opacityVal += 0.5/skinNum;
        QGraphicsPixmapItem* item = new QGraphicsPixmapItem(QPixmap::fromImage(*(renderImg)));
        item->setOffset(onionOffset);
        item->setOpacity(opacityVal);

        item->setZValue(0);
        onionSkins.append(item);
        scene()->addItem(item);
        renderScene.clear();
        painter.end();
    }
    onionRender = false;
}
#define MAXUNDO 25
void myView::undoRedo(int mode){
    undoFlag = true;
    if(mode == MODE_UNDO && undoBuffer.isEmpty())
        return;
    else if(mode == MODE_REDO && redoBuffer.isEmpty())
        return;
    //CLONE FRAME
    // byte array stores serialized data
    QByteArray byteArray;
    // buffer temporarily holds serialized data
    QBuffer buffer1(&byteArray);
    // use this buffer to store data from the object
    buffer1.open(QIODevice::WriteOnly);
    QDataStream myStream(&buffer1);
    undoInfoStruct myUndo;
    if(mode == MODE_REDO){
        myUndo = redoBuffer.takeLast();
    }
    else {
        myUndo = undoBuffer.takeLast();
    }
    myStream<<*(myUndo.frame);
    // now create a seconds buffer from which to read data of the bytearray
    QBuffer buffer2(&byteArray);
    buffer2.open(QIODevice::ReadOnly);
    // a new data stream to deserialize
    QDataStream myStream2(&buffer2);

    Frame* undoFrame = new Frame();
    if(mode == MODE_UNDO){
        storeUndo(myUndo.command,MODE_REDO);
    }
    else if(mode == MODE_REDO){
        storeUndo(myUndo.command,MODE_UNDO);
    }
    // hydrate new frame with previous frame data
    undoFlag = true;
    myStream2>>*undoFrame;
    //scene()->clear();
    if((myUndo.command == CMD_ADDFRAME && mode == MODE_UNDO)||
            (myUndo.command == CMD_DELETEFRAME && mode == MODE_REDO)){
        deleteFrame(myAnimation->currentFrame);
        myAnimation->frameList[undoFrame->frameNumber]->clearFrame();
        myAnimation->cloneFrame(myAnimation->frameList[undoFrame->frameNumber],undoFrame);
        moveToFrame(myAnimation->frameList[undoFrame->frameNumber]);
    }
    else if(myUndo.command == CMD_MOVETOFRAME){
        myAnimation->frameList[undoFrame->frameNumber]->clearFrame();
        myAnimation->cloneFrame(myAnimation->frameList[undoFrame->frameNumber],undoFrame);
        moveToFrame(myAnimation->frameList[undoFrame->frameNumber]);
    }
    else if((myUndo.command == CMD_DELETEFRAME && mode == MODE_UNDO)||
            (myUndo.command == CMD_ADDFRAME && mode == MODE_REDO)){
        Frame* addedFrame = setUpFrame(undoFrame->frameNumber);
        myAnimation->cloneFrame(addedFrame,undoFrame);
        moveToFrame(addedFrame);

    }
    else{
        myAnimation->currentFrame->clearFrame();
        scene()->clear();
        myRect = new QGraphicsRectItem(0,0,W,H); // RENEW IT
        myAnimation->cloneFrame(myAnimation->currentFrame,undoFrame);
        moveToFrame(myAnimation->currentFrame);
    }
    delete undoFrame;
    undoFlag = false;
    //
}
void myView::storeUndo(int command, int mode){
    if(myAnimation->frameList.isEmpty())
        return;
    undoFlag = true;
    undoInfoStruct myUndo;

    myUndo.frame = new Frame();
    myUndo.command = command;

    //CLONE FRAME
    // byte array stores serialized data
    QByteArray byteArray;
    // buffer temporarily holds serialized data
    QBuffer buffer1(&byteArray);
    // use this buffer to store data from the object
    buffer1.open(QIODevice::WriteOnly);
    QDataStream myStream(&buffer1);
    myStream<<*(myAnimation->currentFrame);
    // now create a seconds buffer from which to read data of the bytearray
    QBuffer buffer2(&byteArray);
    buffer2.open(QIODevice::ReadOnly);
    // a new data stream to deserialize
    QDataStream myStream2(&buffer2);
    // hydrate new frame with previous frame data
    myStream2>>*myUndo.frame;
    //
    if(mode == MODE_UNDO){
        if(undoBuffer.count()>=MAXUNDO){
            delete undoBuffer.takeFirst().frame;
        }
        undoBuffer.append(myUndo);
    }
    else if(mode ==MODE_REDO){
        if(redoBuffer.count()>=MAXUNDO){
            delete redoBuffer.takeFirst().frame;
        }
        redoBuffer.append(myUndo);
    }
    undoFlag = false;
}
void myView::clearUndo(){
    clearUndoFlag = true;
    for(undoInfoStruct u:undoBuffer){
        delete u.frame;
    }
    undoBuffer.clear();
    for(undoInfoStruct u:redoBuffer){
        delete u.frame;
    }
    redoBuffer.clear();
    clearUndoFlag = false;
}
