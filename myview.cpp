﻿#include "myview.h"
#include <math.h>
#include <QDebug>
#include <QGraphicsRotation>
#include <frame.h>
#include <QMessageBox>
#include <QSpinBox>
#include <QBuffer>
#include <QCheckBox>
#include <animationoptions.h>
#include <QMessageBox>
#include "uiItems.h"
#define CS myAnimation->currentFrame->currentStickFigure->currentStick
#define CURRENTSTICKFIGURE myAnimation->currentFrame->currentStickFigure
#define CURRENTFRAME myAnimation->currentFrame


bool    isRightPressed = false;
bool    isMiddlePressed = false;
bool    isPressed = false;
extern float zoomLvl;
QPointF startingCoord; //mouse click coordinateBuffer
QPointF coord; //current mouse pos
float   rotationBuffer; //selected item Rotation buffer
float   lengthBuffer;
float zoomLvlBuf;
bool clearUndoFlag = false;
Frame* stickLibraryBuffer;
QList<Frame*> stickLibraryList;
bool undoFlag = false;
bool libFlag = false;
extern QSpinBox * onionSkinSB;
bool startStickFigureRotation = false;
bool startStickFigureScale = false;
bool translateScene = false;
bool rotateScene = false;
bool scaleScene = false;
StickFigure* joinToStickFigure;
StickFigure* splitToStickFigure;;
stick* joinToStick;
stick* splitToStick;
QList<QGraphicsEllipseItem*> handles;
extern QString imageNameBuffer;
extern int W;
extern int H;
extern QGraphicsRectItem* myRect;
extern QGraphicsRectItem* limitRect;
extern bool playBack ;
extern bool loadFile;
extern QPointF onionOffset;
extern QImage * imageDrawBuffer ;
extern QString animationPath;

myView::myView(QWidget *parent) : QGraphicsView(parent)
{
    onionRender = false;
    myAnimation = new Animation();
    myAnimation->scene = scene();
    setMouseTracking(true);
    myPen.setWidth(10);
    myBrush.setStyle(Qt::SolidPattern);
    myBrush.setColor(Qt::gray);
    myPen.setCapStyle(Qt::RoundCap);
    stickLibraryBuffer = new Frame();
}

void myView::setTool(int Tool)
{
    changeTool();

    tool = Tool;
}

void myView::deleteStickFigure()
{
    bool updateCurrentStickFigure =  false;
    if(myStickFigureWidgetList->selectedItems().count() == 1 && myAnimation->currentFrame->stickFigures.count()>1)
    {
        //scegli lo stickfigure selzionato dalla lista grafica
        QListWidgetItem *current = myStickFigureWidgetList->selectedItems()[0];
        // estrai il dato utente salvato in esso, puntatore ad uno stickfigure
        QVariant  retrievedData = (current->data(Qt::UserRole));
        //riconverti il dato
        StickFigure* cs = qvariant_cast<StickFigure*>(retrievedData);
        updateCurrentStickFigure = (cs == CURRENTSTICKFIGURE);

        //distruggi l'oggetto della lista
        myStickFigureWidgetList->takeItem(myStickFigureWidgetList->row(current));
        delete current;
        // delete from the total list
        for(stick*s:cs->stickList){
            CURRENTFRAME->totalSticks.removeAll(s);
        }
        //rimuovilo dal frame e distruggilo
        StickFigure* ns = myAnimation->currentFrame->removeStickFigure(cs);
        // Ora controlliamo la nuova selezione non sia nulla e aggiorniamo la selezione
        // dell'elenco del widget
        if(ns!=NULL && ns!=nullptr)
        {
            myAnimation->updateSelection(ns);
            myStickFigureWidgetList->clearSelection();
            ns->linkedItem->setSelected(true);
        }
        myAnimation->currentFrame->updateIcon();
        if(updateCurrentStickFigure){
             arrowSelection();
        }
    }
    else{
        QMessageBox::about(this,"error","There must be at least one stickfigure layer in the scene");
    }

}
//cambia l'ordine di profondità dello stickfigure selezionato
void myView::moveStickFigureZ(int increment, int mode)
{
    if(myStickFigureWidgetList->count() <= 1 )
        return;
    int currentIndex = myStickFigureWidgetList->currentRow();
    if((currentIndex == 0 && increment<0)||(currentIndex == myStickFigureWidgetList->count()-1 && increment>0))
        return;
    if(mode == MODE_TOP){
        increment = myStickFigureWidgetList->count()-1-currentIndex;
        if(increment == 0 )
            return;
    }
    else if(mode == MODE_BOTTOM){
        increment = -currentIndex;
        if(increment == 0)
            return;
    }
    int newIdx = currentIndex+increment;
    QListWidgetItem *stickFigureAtNewIdxItem = myStickFigureWidgetList->item(newIdx);
    // estrai il dato utente salvato in esso, puntatore ad uno stickfigure
    QVariant  retrievedData = (stickFigureAtNewIdxItem->data(Qt::UserRole));
    //riconverti il dato
    StickFigure* stickFigureAtNewIdx = qvariant_cast<StickFigure*>(retrievedData);

    if(newIdx<myStickFigureWidgetList->count() && newIdx>=0 && newIdx != currentIndex){
        if(mode == MODE_TOP){

            CURRENTFRAME->stickfigureToTop(CURRENTSTICKFIGURE);
        }
        else if(mode == MODE_BOTTOM){
            CURRENTFRAME->stickfigureToBottom(CURRENTSTICKFIGURE);

        }
        else if(increment>0){

            CURRENTFRAME->stickfigureAboveBelow(CURRENTSTICKFIGURE,stickFigureAtNewIdx,1);
        }
        else{
            CURRENTFRAME->stickfigureAboveBelow(CURRENTSTICKFIGURE,stickFigureAtNewIdx,0);

        }
        //sposta nella lista grafica il layer dello stickfigure
        QListWidgetItem * item = myStickFigureWidgetList->takeItem(currentIndex);
        myStickFigureWidgetList->insertItem(newIdx,item);
        item->setSelected(true);
        myStickFigureWidgetList->setCurrentRow(newIdx);

        myAnimation->currentFrame->updateIcon();
        CURRENTFRAME->refresh(0);
    }

/*
    if(myStickFigureWidgetList->count()>0)
    {
        myStickFigureWidgetList->clearSelection();
        int currentIndex = myStickFigureWidgetList->currentRow();
        if(!(currentIndex+1 == myStickFigureWidgetList->count() && increment>0)||
                !(currentIndex == 0 && increment<0))
        {
            CURRENTSTICKFIGURE->baseZ+=increment;
            CURRENTSTICKFIGURE->updateZ(0.1);
            if(!(increment>0 && currentIndex == myStickFigureWidgetList->count()-1) && !(increment<0 && currentIndex == 0))
            {
                //sposta nella lista grafica il layer dello stickfigure
                QListWidgetItem * item = myStickFigureWidgetList->takeItem(currentIndex);
                myStickFigureWidgetList->insertItem(currentIndex+increment,item);
                myStickFigureWidgetList->setSelected(item,true);
                myStickFigureWidgetList->setCurrentRow(currentIndex+increment);
            }
            myAnimation->currentFrame->updateIcon();
        }
    }
*/
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
    if(event->button()==Qt::LeftButton){
        isPressed = true;
    }
    else if(event->button()==Qt::RightButton){
        isRightPressed = true;
    }else if(event->button()==Qt::MiddleButton){
        isMiddlePressed = true;
    }
    // converti le coordinate del mouse in coordinate di scena
    mapMyCoords(coord,event->pos());


    startingCoord   = coord; //buffer per il dragghing del mouse premuto
    if(!isRightPressed && !isMiddlePressed){
    switch(tool)
    {
            case CLAMP:
            case NOTOOL:
            {
                storeUndo();
                arrowSelection();
                startStickFigureRotation = true;
                for(stick*s:CURRENTSTICKFIGURE->stickList){
                    s->angleBuffer2 = s->myLine.angle();
                }
                break;
            }
            case DRAW:
            case DRAWCIRCLE:
            case DRAWRECT:
            case DRAWTRAPEZOID:
            case DRAWTAPER:
            case DRAWIMG:
            {
                drawCmd(&coord,(int)tool);
                break;
            }
            case(ROTATE):
            {
                storeUndo();
                startStickFigureRotation = true;
                for(stick*s:CURRENTSTICKFIGURE->stickList){
                    s->angleBuffer2 = s->myLine.angle();
                }
                break;
            }
            case(SCALE):
            {
                storeUndo();
                startStickFigureScale = true;
                for(stick*s:CURRENTSTICKFIGURE->stickList){
                    s->scaleBuffer = s->myLine.length();
                    s->widthBuffer = s->Pen.width();
                }
                break;
            }
            case(MOVESCENE):
            {
                storeUndo();
                break;
            }
            case(ROTATESCENE):
            {
                storeUndo();
                for(StickFigure* S: myAnimation->currentFrame->stickFigures){
                    //check it is not an empty layer
                    if(S->masterStick && S->stickList.count()>0){
                        S->originPosBuffer = S->masterStick->myLine.p1();
                        S->sceneRotationBuffer = 0;
                        for(stick*s:S->stickList){
                            s->angleBuffer2 = s->myLine.angle();
                        }
                    }
                }
                break;
            }
            case(SCALESCENE):
            {
                for(StickFigure* S: myAnimation->currentFrame->stickFigures){
                    //check it is not an empty layer
                    if(S->masterStick && S->stickList.count()>0){
                        S->originPosBuffer = S->masterStick->myLine.p1();
                        for(stick*s:S->stickList){
                            s->scaleBuffer = s->myLine.length();
                            s->widthBuffer = s->Pen.width();
                        }
                    }
                }
                storeUndo();
                break;
            }
            case(JOIN):{
                if(CURRENTSTICKFIGURE!= nullptr){
                    //check layer is not empty
                    if(CURRENTSTICKFIGURE->stickList.count()>0){
                        joinToStickFigure = CURRENTSTICKFIGURE;
                        joinToStick = joinToStickFigure->currentStick;
                        storeUndo();
                        arrowSelection();

                        if(CURRENTSTICKFIGURE!=joinToStickFigure && CURRENTSTICKFIGURE->stickList.count()>0){
                            removeHandles(joinToStickFigure);
                            mergeStickFigures(joinToStickFigure,joinToStick,CURRENTSTICKFIGURE);
                            addHandles(joinToStickFigure);
                            deleteStickFigure();
                            for(stick*s:CURRENTSTICKFIGURE->stickList){
                                if(!CURRENTFRAME->totalSticks.contains(s))
                                    CURRENTFRAME->totalSticks.append(s);
                            }
                            qDebug("merge ok\r\n");
                        }
                    }
                }
                break;
            }
            case(SPLIT):{

                break;
            }
            case(ELONGATE):
            {
                storeUndo();
                arrowSelection();
                lengthBuffer = CS->myLine.length();
                break;
            }
            case(MANIPULATE):
            {
                storeUndo();
                arrowSelection();
                lengthBuffer = CS->myLine.length();

                break;
            }
    }
    }
    //QGraphicsView::mousePressEvent(event);
}

void myView::keyPressEvent(QKeyEvent *event)
{
    switch(event->key())
    {
        case(Qt::Key_Cancel):
        case(Qt::Key_Delete):
        {
            if(scene()->selectedItems().count() == 1 && CURRENTSTICKFIGURE->drawCount==0)
            {
                storeUndo();

                stick *item = (stick*)scene()->selectedItems()[0];//oggetto selezionato nella scnea
                int idx = CURRENTSTICKFIGURE->stickList.indexOf(item);        // indice corrispondente nella lista ordinata degli oggetti
                // se si tratta dello stick master e ci sono altri stickfigure, cancella l'intero stickfigure
                if(CURRENTSTICKFIGURE->stickList[idx]->isMaster() && // todo master replacement
                        myAnimation->currentFrame->stickFigures.count()>1)
                {
                    //estrai dalla lista grafica l'elemento selezionato collegato allo stickfigure
                    QListWidgetItem * associatedItem = CURRENTSTICKFIGURE->linkedItem;
                    myStickFigureWidgetList->clearSelection();
                    associatedItem->setSelected(true);
                    //cancella lo stickfigure
                    deleteStickFigure();
                }
                //altrimenti cancella il singolo stick e aggiorna l'iconea del livello
                else if(myStickFigureWidgetList->selectedItems().count()==1)
                {
                    stick* nextCurrentStick = CS->parent;
                    // questa lista serve per rimuovere dalla totalStick tutti gli stick cancellati
                    QList<stick*> oldStickList = CURRENTSTICKFIGURE->stickList;
                    CURRENTSTICKFIGURE->deleteStick( idx);
                    QList<stick*> survivors = CURRENTSTICKFIGURE->stickList;
                    for(stick*s:oldStickList){
                        if(!survivors.contains(s)){
                            CURRENTFRAME->totalSticks.removeAll(s);
                        }
                    }
                    CURRENTFRAME->numOfItems = CURRENTFRAME->totalSticks.length();
                    CURRENTSTICKFIGURE->updateIcon();
                    myStickFigureWidgetList->selectedItems()[0]->setIcon(*CURRENTSTICKFIGURE->stickFigureIcon);
                    CS = nextCurrentStick;
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
                    if(CURRENTSTICKFIGURE->drawCount==1)
                    {
                        scene()->removeItem((QGraphicsItem*)CS);
                        CURRENTSTICKFIGURE->cancelDrawing();
                    }
                    break;
                }
            }
            break;
        }
    }

    QGraphicsView::keyPressEvent(event);
}
void myView::arrowSelection(){
    if(CURRENTFRAME->stickFigures.isEmpty())
        return;
    if(CURRENTFRAME->totalSticks.empty())
        return;
    myAnimation->updateSelection(coord);
    myStickFigureWidgetList->clearSelection();
    CURRENTSTICKFIGURE->linkedItem->setSelected(true);
    myStickFigureWidgetList->setCurrentRow(myStickFigureWidgetList->row(CURRENTSTICKFIGURE->linkedItem));
    if(CS!=nullptr && CURRENTSTICKFIGURE->stickList.count()>0)
        CS->populateImageListWidget();
    //dbgLbl->setText(QString::number(CS->order));
}
// il disegno di una linea conta 2 click
// il primo determina lo starting point
// il secondo l'end point
void myView::drawCmd(QPointF* point, int mode)
{
    if(!myAnimation->currentFrame->stickFigures.isEmpty() && CURRENTSTICKFIGURE != nullptr)
    {

        if(CURRENTSTICKFIGURE->drawCount == 0){
            storeUndo();

            CURRENTSTICKFIGURE->startDrawing(point, myPen, myBrush);
            if(!CURRENTSTICKFIGURE->stickList.isEmpty()){
               CS->selected = false;
               CS->refresh(0);
            }
            CS->selected = true;
            CS->hardTop = hardTopCheck->isChecked();
            CS->hardBottom = hardBottomCheck->isChecked();
            CS->Pen = myPen;
            // se stiamo usando il tool per generare cerchi cambiamo il tipo dello stick
            if(mode == DRAWCIRCLE){
                CS->stickType = stick::StickType::CIRCLE;
            }
            else if(mode == DRAWRECT){
                CS->stickType = stick::StickType::RECT;
                CS->imgWScale = advancedRectSlider->value();
                CS->Pen.setJoinStyle(Qt::MiterJoin);
            }
            else if(mode == DRAWTAPER){
                CS->stickType = stick::StickType::TAPER;
                CS->imgHScale = taperHSlider->value();
                CS->imgWScale = taperWSlider->value();
            }
            else if(mode == DRAWTRAPEZOID){
                CS->stickType = stick::StickType::TRAPEZOID;
                CS->imgHScale = taperHSlider->value();
                CS->imgWScale = taperWSlider->value();
                CS->Pen.setJoinStyle(Qt::MiterJoin);
            }
            else if(mode == DRAWIMG){
                CS->stickType = stick::StickType::IMAGE;
                QImage* img = new QImage(*imageDrawBuffer);

                CS->addImage(img, imageNameBuffer);

            }

            //aggiungi il nuovo elemento alla scena mediante puntatore dell'elemento linea dell'oggetto stick
           scene()->addItem(CS);
           qDebug("Draw 1 = %f, %f",point->x(),point->y());
        }
        else if(CURRENTSTICKFIGURE->drawCount >0)
        {
            CURRENTSTICKFIGURE->endDrawing(point);
            scene()->addItem(CS->stickHandle[0]);
            scene()->addItem(CS->stickHandle[1]);
            qDebug("Draw 2 = %f, %f",point->x(),point->y());
            myAnimation->currentFrame->updateIcon();
            myAnimation->updateTab(CS->stickType);
            // add it to the total scene list
            myAnimation->currentFrame->totalSticks.append(CS);
            myAnimation->updateSelection(CS);
            handles.append((QGraphicsEllipseItem*)CS->stickHandle);

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
        case(DRAWRECT):
        case(DRAWTAPER):
        case(DRAWTRAPEZOID):
        case(DRAWIMG):
        {
            myAnimation->updateTab(tool); // updates ui panels
            if(!myAnimation->currentFrame->stickFigures.isEmpty())
            {
                // se stavi disegnando uno stick e cambi prima di averlo finito, cancellalo!
                if(CURRENTSTICKFIGURE->drawCount==1)
                {
                    scene()->removeItem((QGraphicsItem*)CS);
                    CURRENTSTICKFIGURE->cancelDrawing();
                }
            }
            else{

            }
            if(tool == DRAWIMG){
                delete imageDrawBuffer;
                imageDrawBuffer = nullptr;
            }
            break;
        }
        case(MANIPULATE):
        {
            break;
        }
        case(ROTATE):
        {
            break;
        }
        case(SCALE):
        {
            break;
        }
        case(ELONGATE):
        {
            break;
        }
        case(MOVESCENE):
        {
            break;
        }
        case(ROTATESCENE):
        {
            break;
        }
        case(SCALESCENE):
        {
            break;
        }
        case(JOIN):
        {
            break;
        }
        case(SPLIT):
        {
            break;
        }
    }
}

void myView::wheelEvent(QWheelEvent *event)
{
    if(CURRENTSTICKFIGURE !=nullptr){
        if(!CURRENTSTICKFIGURE->stickList.isEmpty() && CURRENTSTICKFIGURE->currentStick!=nullptr){
            int idx = CURRENTFRAME->totalSticks.indexOf(CURRENTSTICKFIGURE->currentStick);
            if(event->angleDelta().y()>0){
                for(int j = idx+1;j<CURRENTFRAME->totalSticks.length();j++){
                    if(CURRENTSTICKFIGURE->stickList.contains(CURRENTFRAME->totalSticks[j])){
                        CURRENTSTICKFIGURE->highlight(false);

                        CURRENTFRAME->selectStick(CURRENTSTICKFIGURE,CURRENTFRAME->totalSticks[j]);
                        break;
                    }
                }
            }
            else{
                for(int j = idx-1;j>-1;j--){
                    if(CURRENTSTICKFIGURE->stickList.contains(CURRENTFRAME->totalSticks[j])){
                        CURRENTSTICKFIGURE->highlight(false);
                        CURRENTFRAME->selectStick(CURRENTSTICKFIGURE,CURRENTFRAME->totalSticks[j]);
                        break;
                    }
                }
            }
            CURRENTSTICKFIGURE->refresh(0);
        }
    }
}



// mouse rilasciato
void myView::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button()==Qt::LeftButton){
        isPressed = false;
        if(myAnimation->currentFrame->stickFigures.count()>0)
        {
            if(CURRENTSTICKFIGURE->stickList.count()>0
                     && myStickFigureWidgetList->selectedItems().count() == 1)
            {
                //termina una rotazione di stick al rilascio
                CS->endRotation();
                CURRENTSTICKFIGURE->updateIcon();
                myStickFigureWidgetList->selectedItems()[0]->setIcon(*CURRENTSTICKFIGURE->stickFigureIcon);
                myAnimation->currentFrame->updateIcon();
            }
        }
    }else if(event->button()==Qt::RightButton){
        isRightPressed = false;
        setTransformationAnchor(QGraphicsView::AnchorViewCenter);
    }else if(event->button()==Qt::MiddleButton){
        isMiddlePressed = false;
        //zoomLvl = zoomLvlBuf/zoomLvl;
        setTransformationAnchor(QGraphicsView::AnchorViewCenter);
    }

    QGraphicsView::mouseReleaseEvent(event);
}

// Se il mouse viene mosso durante il disegno di una linea continua ad aggiornarne il disegno
// Se viene mosso durante una pressione con stick selezionato, agisci sul relativo puntatore all'elemento linea
// per ruotarla seguendo il mouse
void myView::mouseMoveEvent(QMouseEvent *event)
{
    static QPointF prevCoord(0,0);
    mapMyCoords(coord, event->pos());
    switch(tool)
    {
        case CLAMP:
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
                    if(CURRENTSTICKFIGURE->stickList.count()>0){
                        if(tool == CLAMP){
                            CS->rotate(&coord);
                        }
                        else{
                            QList<stick*> toRotate;
                            if(CS->isMaster()){ // // todo master replacement
                                for(stick*s:CURRENTSTICKFIGURE->stickList){
                                    if(!s->stepchild){
                                        toRotate.append(s);
                                    }
                                }
                            }else{
                                toRotate = CS->children;
                            }
                            toRotate.append(CS);
                            CURRENTSTICKFIGURE->rotateStickFigure(&coord,toRotate,false);
                        }
                            //
                    }
                }
            }
            break;
        }
        case DRAW:
        case DRAWCIRCLE:
        case DRAWRECT:
        case DRAWTAPER:
        case DRAWTRAPEZOID:
        case DRAWIMG:
        {
        // durante il disegno,l'estremo libero della linea segue il mouse
            if(CURRENTSTICKFIGURE != nullptr && CURRENTSTICKFIGURE != nullptr)
            {
                if(CURRENTSTICKFIGURE->drawCount > 0)
                {
                    CURRENTSTICKFIGURE->previewDrawing(&coord);
                    qDebug("coord = %f, %f",coord.x(),coord.y());
                }
            }
            else
            {
            }
            break;
        }
        case MANIPULATE:
        {
            if(CURRENTSTICKFIGURE != nullptr && CS!=nullptr)
            {
                if(isPressed){
                    CS->manipulate(&coord);
                    elongateSpinbox->setValue(CS->myLine.length());
                }
            }
            break;
        }
        case ROTATE:
        {
            if(CURRENTSTICKFIGURE != nullptr)
            {
                if(isPressed){
                    CURRENTSTICKFIGURE->rotateStickFigure(&coord,CURRENTSTICKFIGURE->stickList,true);
                }
            }
            break;
        }
        case(SCALE):
        {
            if(CURRENTSTICKFIGURE != nullptr && isPressed){
               CURRENTSTICKFIGURE->scale(&coord);
            }
            break;
        }
        case(MOVESCENE):
        {
            if(isPressed && !myAnimation->currentFrame->stickFigures.isEmpty()){
                int dx = -startingCoord.x()+coord.x();
                int dy = -startingCoord.y()+coord.y();
                startingCoord = coord;
                for(StickFigure*S:myAnimation->currentFrame->stickFigures){
                    //check it is not an empty layer
                    if( S->stickList.count()>0 && S->masterStick){
                        S->traslate(dx,dy);
                    }
                }
            }
            break;
        }
        case(ROTATESCENE):
        {
            if(isPressed && !myAnimation->currentFrame->stickFigures.isEmpty()){
                QPointF center(W/2,H/2); // centro del sistema
                QLineF mouseToCenterNow(coord,center); // linea che connette il centro al mouse
                QLineF mouseToCenterStart(startingCoord,center); // linea che connette il centro alla posizione del mouse precedente
                float startingAngle = mouseToCenterStart.angle(); // angolo assoluto del mouse prima rispetto al centro

                float actualAngle = mouseToCenterNow.angle();// angolo assoluto del mouse ora rispetto al centro
                float angle = actualAngle-startingAngle; // incremento in angolo rispetto a prima
                startingCoord = coord; // le coordinate attuali vengono memorizzate per la prossima iterazione
                for(StickFigure*S:myAnimation->currentFrame->stickFigures){
                    //check it is not an empty layer
                    if( S->stickList.count()>0 && S->masterStick){
                        QLineF connectCenter(center,S->masterStick->myLine.p1()); // linea che connette il master dello stick al centro

                        connectCenter.setAngle(connectCenter.angle()+angle); // ruotiamola dell'angolo di incremento
                        // ruota ogni singolo stick
                        for(stick *s:S->stickList){

                            s->rotate(angle);
                            s->angleBuffer2+=angle; // aggiorna il buffer interno dell'angolo dello stick
                        }

                        QLineF Delta(S->originPosBuffer,connectCenter.p2()); // traslazione della punta della lineacentro-master ruotata
                        //trasla lo stickfigure dell'ammontare
                        S->traslate(Delta.dx(),Delta.dy());
                        S->originPosBuffer = S->masterStick->myLine.p1(); // agiorna il buffer di posizione dello stickfigure
                    }
                    }
            }
            break;
        }
        case(SCALESCENE):
        {


            if(isPressed && !myAnimation->currentFrame->stickFigures.isEmpty()){
                float scaleAmount = 1;
                float dist = 0;
                QPointF center(W/2,H/2);
                float startDist = QLineF(startingCoord,center).length();
                dist = QLineF(coord,center).length();
                scaleAmount = dist/startDist;
                for(StickFigure * S:myAnimation->currentFrame->stickFigures){
                    //check it is not an empty layer
                        if( S->stickList.count()>0 && S->masterStick){
                        // per traslare lo stick verso il centro [W/2,H/2] proporzionalmente
                        // serve calcolare la distanza del masterstick da essa
                        // moltiplicare dx e dy per lo scale factor
                        // usarli come traslazione
                        float dx = 0;
                        float dy = 0;
                        // il dx e dy deve essere una quantità tale da non far eccedere ad actual dist ilv alore  di originaldist
                        QLineF actualDistFromCenter(S->masterStick->myLine.p1(),center);
                        QLineF originalDistFromCenter(S->originPosBuffer,center);
                        // quindi fai la differenza fra il valore di distaza originale (max) e la distanza attuale modificata dal fattore di scala per dx e dy
                        dx = -originalDistFromCenter.dx()*scaleAmount+actualDistFromCenter.dx();
                        dy = -originalDistFromCenter.dy()*scaleAmount+actualDistFromCenter.dy();
                        S->traslate(dx,dy);
                        for(stick* s:S->stickList){
                            s->scale(scaleAmount);

                        }
                    }
                }
            }
            break;
        }
        case(ELONGATE):
        {
            if(CURRENTSTICKFIGURE==nullptr)
                break;
            if(CS == nullptr)
                break;
            if(isPressed){
                QLineF newEndPoint(CS->myLine.p1(),coord);
                lengthBuffer = newEndPoint.length();
                CURRENTSTICKFIGURE->elongate(coord,CS);
                elongateSpinbox->setValue(lengthBuffer);
            }
            break;
        }

    }
    if(isRightPressed){
        setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
        QLineF travel(prevCoord,coord);
        translate(travel.dx(),travel.dy());
    }else if(isMiddlePressed){

        //calcola lo scostamento dal centro all'inizio della pressione e quella attuale
        float distFromCenter0 = QLineF(prevCoord,QPointF(W/2,H/2)).length();
        float distFromCenter1 = QLineF(coord,QPointF(W/2,H/2)).length();

        zoomLvlBuf = zoomLvl+(distFromCenter1-distFromCenter0)/(0.5*sqrtf(W*W+H*H)); // zoombuf è incrementato dal valore precedente di un valore sempre minore di 1

        scale(zoomLvlBuf/zoomLvl,zoomLvlBuf/zoomLvl); // la divisione serve a riportare allo zoom 0, lo zoom applicato è zombuf
        zoomLvl = zoomLvlBuf;
    }
    prevCoord = coord;
    QGraphicsView::mouseMoveEvent(event);
}
//funzione usata per riscalare interamente il frame quando si riscala l'animazione intera
void myView::scaleFrame(Frame* F, float scaleAmount, QRectF R){
    QPointF center(R.width()/2,R.height()/2);
    if(F->stickFigures.isEmpty())
        return;
    for(StickFigure * S:F->stickFigures){
        if(!S->stickList.isEmpty()){
            QLineF actualDistFromCenter(S->masterStick->myLine.p1(),center);
            float dx =-(scaleAmount-1)*actualDistFromCenter.dx();
            float dy =-(scaleAmount-1)*actualDistFromCenter.dy();
            S->traslate(dx,dy);
            for(stick* s:S->stickList){
                s->scaleBuffer = s->myLine.length();
                s->widthBuffer = s->Pen.widthF();
                s->scale(scaleAmount);
            }
        }

    }
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
    myAnimation->currentFrame->linkedItem->setSelected(true);
}
// routine per passare da un frame ad un altro, svuota la scena, svuota le widget list, ripopola entrambi, aggiorna onion skins e selezioni
void myView::moveToFrame(Frame* frame){
    //myAnimation->currentFrame->updateRender();
    QRectF myR = scene()->sceneRect();
    stick* orderedStickList[frame->numOfItems];
    int j = 0;
    int r = 0;
    // rimuovi tutti gli stick dalla scena
    if(myAnimation->frameList.count()>=1){
        for(StickFigure *S:myAnimation->currentFrame->stickFigures){
            removeHandles(S);
            for(stick * s: S->stickList){
               if(scene()->items().contains(s)){
                    scene()->removeItem(s);
                    //s->order = j;
                    j++;
               }
            }
        }
    }
    // rimuovigli elementi dalla lista widget
    myStickFigureWidgetList->clear();

    // aggiungi tutta gli stickfigure del nuovo frame alla lista del widget
    for(StickFigure *S:frame->stickFigures){
        QListWidgetItem * addedItem = new QListWidgetItem(myStickFigureWidgetList);
        QVariant newData(QVariant::fromValue(S));
        addedItem->setData(Qt::UserRole,newData);
        addedItem->setData(Qt::DisplayRole,S->name);
        addedItem->setIcon(*S->stickFigureIcon);
        S->linkedItem = addedItem;
        myStickFigureWidgetList->addItem(S->linkedItem);
        // aggiungi alla scena
        if(!playBack){
            S->updateBoundingRects();
            S->updateIcon();
            addHandles(S);
        }
    }
    scene()->clearSelection();
    myAnimation->currentFrame = frame;
    for(int j = 0;j<frame->numOfItems;j++){
        scene()->addItem(frame->totalSticks[j]);

        //orderedStickList[frame->numOfItems-1-j]->refresh(0);
    }

    // aggiorna il current frame, se si parte d auna situazione di inizio forza il primo stickfigure
    if(frame->currentStickFigure !=nullptr){
        if(frame->currentStickFigure->currentStick!=nullptr){
            frame->selectStick(frame->currentStickFigure,frame->currentStickFigure->currentStick);
        }
        else{
            arrowSelection();
        }
    }
    else if(frame->stickFigures.count() == 1 && frame->stickFigures[0]->stickList.isEmpty()){
        CURRENTSTICKFIGURE = frame->stickFigures[0];
        myAnimation->updateSelection(CURRENTSTICKFIGURE);
    }else{
        arrowSelection();
    }
    /*
    if(!myAnimation->currentFrame->stickFigures.isEmpty())
        CURRENTSTICKFIGURE = myAnimation->currentFrame->stickFigures[0];
    // forza la selezione dell' item in posizione 0
    myStickFigureWidgetList->clearSelection();
    myFrameWidgetList->clearSelection();

    if(!myAnimation->currentFrame->stickFigures.isEmpty())
        if(frame->numOfItems>0)
            myAnimation->updateSelection(myAnimation->currentFrame->stickFigures.last());
            */
    if(!playBack)
        updateOnionSkins();//aggiorna gli onion skins

    //scene()->update(myR);
    // se non siamo in modalità playback aggiorna le selezioni nelle widgetlists e delle iconee
    if(playBack == false){
        myAnimation->currentFrame->linkedItem->setSelected(true);
        myFrameWidgetList->setCurrentItem(myAnimation->currentFrame->linkedItem);
        if(!myAnimation->frameList.isEmpty() && !myAnimation->currentFrame->stickFigures.isEmpty()){
            //CURRENTSTICKFIGURE->updateIcon();
            myFrameWidgetList->selectedItems()[0]->setIcon(*myAnimation->currentFrame->frameIcon);
            myAnimation->currentFrame->updateIcon();
        }
        if(myRect == nullptr){
            myRect = new QGraphicsRectItem(0,0,W,H);
            myRect->setPen(Qt::NoPen);
            myRect->setZValue(-100);
            if(!scene()->items().contains(myRect))
                scene()->addItem(myRect);
        }


    }
    CURRENTFRAME->refresh(0);

}
// routine per calcolare gli onion skins, prima cancellali, ridisegnali su un immagine, e aggiungi l'immagine alla scena
void myView::updateOnionSkins(){
    // svuota i buffer di immagine vecchi (presneti nella scena) immagazzinati nel ciclo for precedente
    for(QGraphicsPixmapItem* P:onionSkins){
        scene()->removeItem(P);
        delete P;
    }
    onionSkins.clear();
    if(onionSkinSB->value()==0 || myAnimation->currentFrame->frameNumber == 0)
        return;
    // imposta la flag per routine speciali
    onionRender = true;
    QGraphicsScene renderScene; // scena fittizia su cui renderizzare gli onionskins
    // immagine fittizia su cui registrare gli onion skins
    QImage *renderImg = new QImage(scene()->sceneRect().width(),scene()->sceneRect().height(),QImage::Format_ARGB32);

    renderScene.setSceneRect(scene()->sceneRect());
    // ora clona tutti gli stick nello stickfigure in una tempList, aggiungili alla scena fittizia
    QList<stick*> tempList;
    // dallo spinbox trai il valore del numero di onionskins servono
    int skinNum = onionSkinSB->value();
    float opacityVal = 0;
    if(myAnimation->currentFrame->frameNumber<skinNum)
        skinNum = myAnimation->currentFrame->frameNumber;
    // disegna sulla scena fittizia gli stickfigures dei frame da onionskinnare che partono dal frame corrente- numero di skin calcolabili
    for(int i = myAnimation->currentFrame->frameNumber-skinNum; i<myAnimation->currentFrame->frameNumber; i++){

        for(stick *s:myAnimation->frameList[i]->totalSticks)
        {
            stick* clone = new stick(s);
            tempList.append(clone);
            renderScene.addItem(clone);
        }
        QPainter painter(renderImg);
        renderImg->fill(Qt::transparent);
        painter.setBackground(QBrush(QColor(Qt::transparent)));
        renderScene.render(&painter);
        // più è vecchio l'onion skin più è trasparente, più è vicino al frame corrente più è opaco
        opacityVal += 0.5/skinNum;
        // trasferisci la scena renderizzata
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
// Questa funzione fa sia undo che redo, a seconda prende come sorgente un buffer diverso undoBuffe ro redoBuffer
// copia dal Buffer,
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
    if(mode == MODE_UNDO){// memorizza l'undo nel buffer dei redo
        storeUndo(myUndo.command,MODE_REDO);
    }
    else if(mode == MODE_REDO){ // se siamo in redo immetti lo stato negli undo
        storeUndo(myUndo.command,MODE_UNDO);
    }
    // hydrate new frame with previous frame data
    undoFlag = true;
    myStream2>>*undoFrame;
    //scene()->clear();
    // se è un undo su un addframe, o un redo su un deleteframe ci sta da rimuovere un frame
    if((myUndo.command == CMD_ADDFRAME && mode == MODE_UNDO)||
            (myUndo.command == CMD_DELETEFRAME && mode == MODE_REDO)){
        // routine di cancellazione del frame
        deleteFrame(myAnimation->currentFrame);
        // svuota il frame da popolare con il buffer
        myAnimation->frameList[undoFrame->frameNumber]->clearFrame();
        //popola il frame di destinazione con il buffer
        myAnimation->cloneFrame(myAnimation->frameList[undoFrame->frameNumber],undoFrame);
        // spostati verso tale frame
        moveToFrame(myAnimation->frameList[undoFrame->frameNumber]);
    }
    // se è un undo su uno spostamento
    else if(myUndo.command == CMD_MOVETOFRAME){
        //svuota il frame di destinazione
        myAnimation->frameList[undoFrame->frameNumber]->clearFrame();
        // popolalo col buffer
        myAnimation->cloneFrame(myAnimation->frameList[undoFrame->frameNumber],undoFrame);
        // spostati sul frame di destinazione
        moveToFrame(myAnimation->frameList[undoFrame->frameNumber]);
    }
    // se è un undo su un deleteframe o un redo su un adddframe bisogna aggiungere un frame
    else if((myUndo.command == CMD_DELETEFRAME && mode == MODE_UNDO)||
            (myUndo.command == CMD_ADDFRAME && mode == MODE_REDO)){
        // routine di aggiunta del frame
        Frame* addedFrame = setUpFrame(undoFrame->frameNumber);
        // popola il frame aggiuntio col buffer
        myAnimation->cloneFrame(addedFrame,undoFrame);
        // spostati sulla destinazione
        moveToFrame(addedFrame);

    }
    else{
        myAnimation->currentFrame->clearFrame();
        sceneRemover(scene());
        //scene()->clear();
        //myRect = new QGraphicsRectItem(0,0,W,H); // RENEW IT
        myAnimation->cloneFrame(myAnimation->currentFrame,undoFrame);
        moveToFrame(myAnimation->currentFrame);
    }
    delete undoFrame;
    undoFlag = false;
    //
}
extern int autoSaveInterval;
extern int opCount;
// routine per immagazzinare undo e redo popolando il buffer a seconda della mode selezionata
// inoltre incrementiamo il contatore delle operazioni e verifichiamo se possiamo innescare l'autosave
void myView::storeUndo(int command, int mode){
    if(myAnimation->frameList.isEmpty())
        return;
    updateFrameOrder(CURRENTFRAME);
    opCount++;
    qDebug() << "beforetrigger autosav";
    // se il file è stato salvato, ed esiste, e sono passati un numero sufficiente di istruzioni, procedi con láutosave
    if(QFile::exists(animationPath) && opCount%autoSaveInterval == 0){
        emit autoSaveTrigger();
    }
    qDebug() << "after trigger autosav";
    undoFlag = true;
    // inizializza la struttura di undo/redo
    undoInfoStruct myUndo;
    myUndo.frame = new Frame();
    myUndo.command = command;
    qDebug() << "after new frame";
    //CLONE FRAME
    // byte array stores serialized data
    QByteArray byteArray;
    // buffer temporarily holds serialized data
    QBuffer buffer1(&byteArray);
    // use this buffer to store data from the object
    buffer1.open(QIODevice::WriteOnly);
    qDebug() << "open buffer";
    QDataStream myStream(&buffer1);

    qDebug() << "after stream obj";
    myStream<<*(myAnimation->currentFrame);
    buffer1.close();
    qDebug() << "stream frame";
    // now create a seconds buffer from which to read data of the bytearray
    QBuffer buffer2(&byteArray);
    buffer2.open(QIODevice::ReadOnly);
    // a new data stream to deserialize
    qDebug() << "buffer 2";
    QDataStream myStream2(&buffer2);

    // hydrate new frame with previous frame data
    myStream2>>*myUndo.frame;
    buffer2.close();
    qDebug() << "after clone frame";
    //popola il buffer appropriato
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
    qDebug() << "after uno/redo store";
    undoFlag = false;

}
void myView::updateFrameOrder(Frame* f){
    int j = 0;
    for(stick*s:CURRENTFRAME->totalSticks){
        s->order = j;
        j++;
    }
    CURRENTFRAME->numOfItems = CURRENTFRAME->totalSticks.length();
    /*
    int j = 0;
    QList<QGraphicsItem*> orderedList = f->scene->items(f->scene->itemsBoundingRect(),Qt::IntersectsItemBoundingRect,Qt::DescendingOrder,QTransform());
    for(QGraphicsItem* item:orderedList){
        if(item->type() != STKTYPE){
            orderedList.removeAll(item);
        }
    }
    for(QGraphicsItem* item:orderedList){
        if(item->type() == STKTYPE){
            static_cast<stick*>(item)->order = orderedList.indexOf(item);
            j++;
        }
    }
    f->numOfItems = j;
    */
}

void myView::addHandles(StickFigure *S)
{
    for(stick*s:S->stickList){
        scene()->addItem(s->stickHandle[0]);
        scene()->addItem(s->stickHandle[1]);
        handles.append((QGraphicsEllipseItem*)s->stickHandle);
        if(s->isMaster()){ // todo master replacement
            s->stickHandle[0]->setBrush(QBrush(Qt::green));
            s->stickHandle[0]->setZValue(101);
        }
    }
}

void myView::removeHandles(StickFigure *S)
{

    for(stick*s:S->stickList){
        scene()->removeItem(s->stickHandle[0]);
        scene()->removeItem(s->stickHandle[1]);
        handles.removeAll((QGraphicsEllipseItem*)s->stickHandle);
    }
}
// cancella i buffer di undo e redo, da chiamare quando si carica un nuovo file
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
void myView::saveLibrary(QString fileName){
    libFlag = true;
    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly)){
        libFlag = false;
        return;
    }
    QDataStream out(&file);
    out << *stickLibraryBuffer;
    file.close();
    libFlag = false;
}
void myView::clearCurrentLib(){
    libFlag = true;
    myCurrentLibraryWidget->clear();
    delete stickLibraryBuffer;
    stickLibraryBuffer = new Frame();
    libFlag = false;
}
void myView::loadLibrary(QString fileName, int mode){
    libFlag = true;
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly)){
        libFlag = false;
        return;
    }
    QDataStream out(&file);
    myCurrentLibraryWidget->clear();
    delete stickLibraryBuffer;
    stickLibraryBuffer = new Frame();
    out >> *stickLibraryBuffer;
    // aggiungi tutta gli stickfigure del nuovo frame alla lista del widget

    for(StickFigure *S:stickLibraryBuffer->stickFigures){
        QListWidgetItem * addedItem = new QListWidgetItem();
        QVariant newData(QVariant::fromValue(S));
        addedItem->setData(Qt::UserRole,newData);
        addedItem->setData(Qt::DisplayRole,S->name);
        addedItem->setIcon(*S->stickFigureIcon);
        S->linkedItem = addedItem;
        if(!mode)
            myCurrentLibraryWidget->addItem(S->linkedItem); // questa operazione avviene con lóp >> il widget viene assegnato a seconda della libflag;
    }


    file.close();
    libFlag = false;
}
StickFigure* myView::addStickFigureToLibrary(){
    if(CURRENTSTICKFIGURE->stickList.count()>0){
        libFlag = true;
        stickLibraryBuffer->addStickFigureToLibrary(CURRENTSTICKFIGURE);

        libFlag = false;
    }
}
void myView::removeFromLibrary(){
    libFlag = true;
    // get selected item
    int idx = myCurrentLibraryWidget->currentRow();
    QListWidgetItem *itemToRemove = myCurrentLibraryWidget->takeItem(idx);
    // estrai il dato utente salvato in esso, puntatore ad uno stickfigure
    QVariant  retrievedData = (itemToRemove->data(Qt::UserRole));
    //riconverti il dato
    StickFigure* toRemove = qvariant_cast<StickFigure*>(retrievedData);

    // delete items
    //since we have libFlag on we have to remove these manually
    delete itemToRemove;
    delete toRemove->iconImg;
    delete toRemove->stickFigureIcon;
    //this also deletes stickfigure
    stickLibraryBuffer->removeStickFigure(toRemove);

    libFlag = false;
}
void myView::setGraphics(bool all, int attribute){
    storeUndo(CMD_SIMPLE);
    if(!all){
        if(attribute == ATTRIBUTE_PENCOLOR){
            CS->Pen.setColor(myPen.color());
        }
        else if(attribute== ATTRIBUTE_PENWIDTH)
            CS->Pen.setWidth(myPen.width());
        else if(attribute== ATTRIBUTE_BRUSHCOLOR)
            CS->Brush.setColor(myBrush.color());
        CS->refresh(0);
    }
    else{
        for(stick * s:CURRENTSTICKFIGURE->stickList){
            if(attribute == ATTRIBUTE_PENCOLOR)
                s->Pen.setColor(myPen.color());
            else if(attribute== ATTRIBUTE_PENWIDTH)
                s->Pen.setWidth(myPen.width());
            else if(attribute== ATTRIBUTE_BRUSHCOLOR)
                s->Brush.setColor(myBrush.color());
            s->refresh(0);
        }
    }

    CURRENTSTICKFIGURE->updateIcon();
}
void myView::splitStickFigure(){
    if(CURRENTSTICKFIGURE== nullptr || CS == nullptr)
        return;
    if(CS->isMaster()){ // todo master replacement
        QMessageBox::about(this,"error","cannot split a stickfigure from his master node");
        return;
    }
    storeUndo();
    splitToStickFigure = CURRENTSTICKFIGURE;
    removeHandles(splitToStickFigure);
    splitToStick = splitToStickFigure->currentStick;
    QList<stick*> oldStickList = CURRENTSTICKFIGURE->stickList;

    StickFigure * branch = myAnimation->currentFrame->addStickFigure(myStickFigureWidgetList, splitToStickFigure->name+"_split");
    splitStickFigures(splitToStickFigure,splitToStick,branch);
    splitToStickFigure->refresh(0);
    // update the total list
    QList<stick*> survivorList = splitToStickFigure->stickList;
    for(stick*s:oldStickList){
        if(!survivorList.contains(s)){
            CURRENTFRAME->totalSticks.removeAll(s);
        }
    }
    CURRENTFRAME->totalSticks.append(branch->stickList);
    addHandles(splitToStickFigure);
    addHandles(branch);
    //
    branch->refresh(0);
    CURRENTSTICKFIGURE = branch;
    CS = branch->stickList[0];
    CURRENTSTICKFIGURE->updateIcon();

}
void myView::sizeChange(int option){

    QRectF oldRect = myRect->rect();
    QPointF oldCenter = myRect->rect().center();
    float amount =  W/myRect->rect().width();
    myRect->setPen(Qt::NoPen);
    myRect->setBrush(QBrush(QColor(Qt::white)));
    scene()->setBackgroundBrush(QBrush(QColor(Qt::white)));
    myRect->setRect(0,0,W,H);
    onionOffset = QPointF(-myRect->rect().width()/10,-myRect->rect().height()/10);
    QPointF newCenter = myRect->rect().center();
    limitRect->setRect(-myRect->rect().width()/10, -myRect->rect().height()/10, myRect->rect().width()*1.1, myRect->rect().height()*1.1);
    scene()->setSceneRect(limitRect->rect());
    float dx = newCenter.x()-oldCenter.x();
    float dy = newCenter.y()-oldCenter.y();
    switch(option){

        case REPOSCALE:

            for(Frame*F:myAnimation->frameList){
                scaleFrame(F,amount,oldRect);
                for(StickFigure* S: F->stickFigures){
                    S->traslate(dx,dy);
                }

            }
            break;
        case REPO:
            for(Frame*F:myAnimation->frameList){
                for(StickFigure* S: F->stickFigures){
                    S->traslate(dx,dy);
                }
            }
            break;
        case NOREPO:
            break;
    }

    //()->addItem(myRect);
   // scene()->update(0,0,1000,1000);
}
void myView::preparePreview(){

    playBack = true;
    for(Frame* f :myAnimation->frameList){
        QGraphicsScene renderScene; //scena fittizia
        QImage *renderImg = new QImage(W,H,QImage::Format_ARGB32); //immagine temporanea


        QPainter painter(renderImg); // painter che esegue il drawing sull immagine
        renderScene.setSceneRect(myRect->rect());
        // ora clona tutti gli stick nello stickfigure in una tempList, aggiungili alla scena fittizia
        QList<stick*> tempList;
        for(StickFigure *S:f->stickFigures)
        {
            for(stick* s: S->stickList){
                stick* clone = new stick(s);
                tempList.append(clone);
                renderScene.addItem(clone);
            }
        }
        //disegna sull' immagine e salvala
        renderImg->fill(Qt::white);
        painter.setBackground(QBrush(QColor(Qt::white)));
        renderScene.render(&painter);
        painter.end();
        QGraphicsPixmapItem* item = new QGraphicsPixmapItem(QPixmap::fromImage(*(renderImg)));
        previewList.append(item);
        delete renderImg;
        renderScene.clear();
        //...add all other video frames here
    }
    playBack = false;
}
void myView::displayPreview(int idx){
    QGraphicsPixmapItem* renderImg = previewList[idx];

    //item->setOffset(onionOffset);
    scene()->addItem(renderImg);
    //item->setZValue(0);
}
