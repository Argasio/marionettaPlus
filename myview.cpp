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
Frame* stickLibraryBuffer;
QList<Frame*> stickLibraryList;
extern QListWidget * myStickFigureWidgetList;
extern QListWidget * myFrameWidgetList;
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
extern int W;
extern int H;
extern QGraphicsRectItem* myRect;
extern bool playBack ;
extern bool loadFile;
extern QPointF onionOffset;
extern QImage * imageDrawBuffer ;
extern QSlider* imgHOffsetSlider;
extern QSlider* imgVOffsetSlider;
extern QSlider* imgWidthSlider;
extern QSlider* imgHeightSlider;
extern QSlider* taperWSlider;
extern QSlider* advancedRectSlider;
extern QSlider* taperHSlider;
extern QListWidget * myLibraryListWidget ;
extern QListWidget * myCurrentLibraryWidget;
extern QSlider * depthSlider;
extern QDoubleSpinBox* depthSpinbox;
myView::myView(QWidget *parent) : QGraphicsView(parent)
{
    onionRender = false;
    myAnimation = new Animation();
    myAnimation->scene = scene();
    setMouseTracking(true);
    myPen.setWidth(10);
    myBrush.setStyle(Qt::SolidPattern);
    myPen.setCapStyle(Qt::RoundCap);
    stickLibraryBuffer = new Frame();
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
                for(stick*s:myAnimation->currentFrame->currentStickFigure->stickList){
                    s->angleBuffer2 = s->myLine.angle();
                }
                break;
            }
            case(SCALE):
            {
                storeUndo();
                startStickFigureScale = true;
                for(stick*s:myAnimation->currentFrame->currentStickFigure->stickList){
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
                    S->originPosBuffer = S->masterStick->myLine.p1();
                    S->sceneRotationBuffer = 0;
                    for(stick*s:S->stickList){
                        s->angleBuffer2 = s->myLine.angle();
                    }
                }
                break;
            }
            case(SCALESCENE):
            {
                for(StickFigure* S: myAnimation->currentFrame->stickFigures){
                    S->originPosBuffer = S->masterStick->myLine.p1();
                    for(stick*s:S->stickList){
                        s->scaleBuffer = s->myLine.length();
                        s->widthBuffer = s->Pen.width();
                    }
                }
                storeUndo();
                break;
            }
            case(JOIN):{
                if(myAnimation->currentFrame->currentStickFigure!= nullptr){


                    joinToStickFigure = myAnimation->currentFrame->currentStickFigure;
                    joinToStick = joinToStickFigure->currentStick;
                    storeUndo();
                    myAnimation->updateSelection(coord);
                    myStickFigureWidgetList->clearSelection();
                    myStickFigureWidgetList->setItemSelected(myAnimation->currentFrame->currentStickFigure->linkedItem,true);

                    if(myAnimation->currentFrame->currentStickFigure!=joinToStickFigure){
                        mergeStickFigures(joinToStickFigure,joinToStick,myAnimation->currentFrame->currentStickFigure);
                        deleteStickFigure();
                        qDebug("merge ok\r\n");
                    }
                }
                break;
            }
            case(SPLIT):{

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
            if(scene()->selectedItems().count() == 1 && myAnimation->currentFrame->currentStickFigure->drawCount==0)
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
            if(!myAnimation->currentFrame->currentStickFigure->stickList.isEmpty()){
               myAnimation->currentFrame->currentStickFigure->currentStick->selected = false;
               myAnimation->currentFrame->currentStickFigure->currentStick->refresh(0);
            }
            myAnimation->currentFrame->currentStickFigure->startDrawing(point, myPen, myBrush);
            myAnimation->currentFrame->currentStickFigure->currentStick->selected = true;
            myAnimation->currentFrame->currentStickFigure->currentStick->Pen = myPen;
            // se stiamo usando il tool per generare cerchi cambiamo il tipo dello stick
            if(mode == DRAWCIRCLE){
                myAnimation->currentFrame->currentStickFigure->currentStick->type = stick::stickType::CIRCLE;
            }
            else if(mode == DRAWRECT){
                myAnimation->currentFrame->currentStickFigure->currentStick->type = stick::stickType::RECT;
                myAnimation->currentFrame->currentStickFigure->currentStick->imgWScale = advancedRectSlider->value();
                myAnimation->currentFrame->currentStickFigure->currentStick->Pen.setJoinStyle(Qt::MiterJoin);
            }
            else if(mode == DRAWTAPER){
                myAnimation->currentFrame->currentStickFigure->currentStick->type = stick::stickType::TAPER;
                myAnimation->currentFrame->currentStickFigure->currentStick->imgHScale = taperHSlider->value();
                myAnimation->currentFrame->currentStickFigure->currentStick->imgWScale = taperWSlider->value();
            }
            else if(mode == DRAWTRAPEZOID){
                myAnimation->currentFrame->currentStickFigure->currentStick->type = stick::stickType::TRAPEZOID;
                myAnimation->currentFrame->currentStickFigure->currentStick->imgHScale = taperHSlider->value();
                myAnimation->currentFrame->currentStickFigure->currentStick->imgWScale = taperWSlider->value();
                myAnimation->currentFrame->currentStickFigure->currentStick->Pen.setJoinStyle(Qt::MiterJoin);
            }
            else if(mode == DRAWIMG){
                myAnimation->currentFrame->currentStickFigure->currentStick->type = stick::stickType::IMAGE;
                myAnimation->currentFrame->currentStickFigure->currentStick->stickImg = new QImage(*imageDrawBuffer);

            }

            //aggiungi il nuovo elemento alla scena mediante puntatore dell'elemento linea dell'oggetto stick
           scene()->addItem(myAnimation->currentFrame->currentStickFigure->currentStick);
           qDebug("Draw 1 = %f, %f",point->x(),point->y());
        }
        else if(myAnimation->currentFrame->currentStickFigure->drawCount >0)
        {
            myAnimation->currentFrame->currentStickFigure->endDrawing(point);
            qDebug("Draw 2 = %f, %f",point->x(),point->y());
            myAnimation->currentFrame->updateIcon();
            myAnimation->updateTab(myAnimation->currentFrame->currentStickFigure->currentStick->type);
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
                if(myAnimation->currentFrame->currentStickFigure->drawCount==1)
                {
                    scene()->removeItem((QGraphicsItem*)myAnimation->currentFrame->currentStickFigure->currentStick);
                    myAnimation->currentFrame->currentStickFigure->cancelDrawing();
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
        case DRAWRECT:
        case DRAWTAPER:
        case DRAWTRAPEZOID:
        case DRAWIMG:
        {
        // durante il disegno,l'estremo libero della linea segue il mouse
            if(myAnimation->currentFrame->currentStickFigure != nullptr && myAnimation->currentFrame->currentStickFigure != nullptr)
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
        case MANIPULATE:
        {
            if(myAnimation->currentFrame->currentStickFigure != nullptr)
            {
                if(isPressed){
                    myAnimation->currentFrame->currentStickFigure->currentStick->manipulate(&coord);
                }
            }
            break;
        }
        case ROTATE:
        {
            if(myAnimation->currentFrame->currentStickFigure != nullptr)
            {
                if(isPressed){
                    myAnimation->currentFrame->currentStickFigure->rotateStickFigure(&coord);
                }
            }
            break;
        }
        case(SCALE):
        {
            if(myAnimation->currentFrame->currentStickFigure != nullptr && isPressed)
            {
                myAnimation->currentFrame->currentStickFigure->scale(&coord);
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
                    S->traslate(dx,dy);
                }
                break;
            }
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
// routine per passare da un frame ad un altro, svuota la scena, svuota le widget list, ripopola entrambi, aggiorna onion skins e selezioni
void myView::moveToFrame(Frame* frame){
    //myAnimation->currentFrame->updateRender();
    QRectF myR = scene()->sceneRect();
    int r = 0;
    // rimuovi tutti gli stick dalla scena
    if(myAnimation->frameList.count()>=1){
        for(StickFigure *S:myAnimation->currentFrame->stickFigures){
            for(stick * s: S->stickList){

               scene()->removeItem(s);
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
        for(stick * s: S->stickList){
            scene()->addItem(s);
        }
        if(!playBack){
            S->updateIcon();
        }
    }
    // aggiorna il current frame, se si parte d auna situazione di inizio forza il primo stickfigure
    myAnimation->currentFrame = frame;
    if(!myAnimation->currentFrame->stickFigures.isEmpty())
        myAnimation->currentFrame->currentStickFigure = myAnimation->currentFrame->stickFigures[0];
    // forza la selezione dell' item in posizione 0
    myStickFigureWidgetList->clearSelection();
    myFrameWidgetList->clearSelection();
    scene()->clearSelection();
    if(!myAnimation->currentFrame->stickFigures.isEmpty())
        if(!myAnimation->currentFrame->stickFigures[0]->stickList.isEmpty())
            myAnimation->updateSelection(myAnimation->currentFrame->stickFigures[0]->stickList[0]);

    updateOnionSkins();//aggiorna gli onion skins

    scene()->update(myR);
    // se non siamo in modalità playback aggiorna le selezioni nelle widgetlists e delle iconee
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
// routine per immagazzinare undo e redo popolando il buffer a seconda della mode selezionata
void myView::storeUndo(int command, int mode){
    if(myAnimation->frameList.isEmpty())
        return;
    undoFlag = true;
    // inizializza la struttura di undo/redo
    undoInfoStruct myUndo;
    myUndo.frame = new Frame();
    myUndo.command = command;

    //CLONE FRAME
    // byte array stores serialized data
    QByteArray* byteArray = new QByteArray(myAnimation->currentFrame->stickFigures.count()*20000000,0x00);
    // buffer temporarily holds serialized data
    QBuffer buffer1(byteArray);
    // use this buffer to store data from the object
    buffer1.open(QIODevice::WriteOnly);
    QDataStream myStream(&buffer1);
    myStream<<*(myAnimation->currentFrame);
    // now create a seconds buffer from which to read data of the bytearray
    QBuffer buffer2(byteArray);
    buffer2.open(QIODevice::ReadOnly);
    // a new data stream to deserialize
    QDataStream myStream2(&buffer2);
    // hydrate new frame with previous frame data
    myStream2>>*myUndo.frame;
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
    undoFlag = false;
    delete byteArray;
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
    if(!file.open(QIODevice::WriteOnly))
        return;
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
void myView::loadLibrary(QString fileName){
    libFlag = true;
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly))
        return;
    QDataStream out(&file);
    myCurrentLibraryWidget->clear();
    delete stickLibraryBuffer;
    stickLibraryBuffer = new Frame();
    out >> *stickLibraryBuffer;
    // aggiungi tutta gli stickfigure del nuovo frame alla lista del widget
    /*
    for(StickFigure *S:stickLibraryBuffer->stickFigures){
        QListWidgetItem * addedItem = new QListWidgetItem(myCurrentLibraryWidget);
        QVariant newData(QVariant::fromValue(S));
        addedItem->setData(Qt::UserRole,newData);
        addedItem->setData(Qt::DisplayRole,S->name);
        addedItem->setIcon(*S->stickFigureIcon);
        S->linkedItem = addedItem;
        myCurrentLibraryWidget->addItem(S->linkedItem);
    }*/


    file.close();
    libFlag = false;
}
StickFigure* myView::addStickFigureToLibrary(){
    if(myAnimation->currentFrame->currentStickFigure->stickList.count()>0){
        libFlag = true;
        stickLibraryBuffer->addStickFigureToLibrary(myAnimation->currentFrame->currentStickFigure);

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
            myAnimation->currentFrame->currentStickFigure->currentStick->Pen.setColor(myPen.color());
        }
        else if(attribute== ATTRIBUTE_PENWIDTH)
            myAnimation->currentFrame->currentStickFigure->currentStick->Pen.setWidth(myPen.width());
        else if(attribute== ATTRIBUTE_BRUSHCOLOR)
            myAnimation->currentFrame->currentStickFigure->currentStick->Brush.setColor(myBrush.color());
        myAnimation->currentFrame->currentStickFigure->currentStick->refresh(0);
    }
    else{
        for(stick * s:myAnimation->currentFrame->currentStickFigure->stickList){
            if(attribute == ATTRIBUTE_PENCOLOR)
                s->Pen.setColor(myPen.color());
            else if(attribute== ATTRIBUTE_PENWIDTH)
                s->Pen.setWidth(myPen.width());
            else if(attribute== ATTRIBUTE_BRUSHCOLOR)
                s->Brush.setColor(myBrush.color());
            s->refresh(0);
        }
    }

    myAnimation->currentFrame->currentStickFigure->updateIcon();
}
void myView::splitStickFigure(){
    if(myAnimation->currentFrame->currentStickFigure== nullptr || myAnimation->currentFrame->currentStickFigure->currentStick == nullptr)
        return;
    storeUndo();
    splitToStickFigure = myAnimation->currentFrame->currentStickFigure;
    splitToStick = splitToStickFigure->currentStick;
    StickFigure * branch = myAnimation->currentFrame->addStickFigure(myStickFigureWidgetList, splitToStickFigure->name+"_split");
    splitStickFigures(splitToStickFigure,splitToStick,branch);
    splitToStickFigure->refresh(0);
    branch->refresh(0);
    myAnimation->currentFrame->currentStickFigure = branch;
    myAnimation->currentFrame->currentStickFigure->currentStick = branch->stickList[0];
    myAnimation->currentFrame->currentStickFigure->updateIcon();
}
