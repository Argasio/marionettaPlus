#include "frame.h"
#include <QListWidget>
#include <QDebug>
QListWidget * ListWidget;
extern bool undoFlag;
extern QListWidget * myStickFigureWidgetList;
extern QListWidget * myFrameWidgetList;
Frame::Frame()
{
    frameIcon       = new QIcon();
    iconImg         = new QPixmap(50,50);
}
Frame::~Frame(){
    for(StickFigure* S: stickFigures)
    {
        delete S;
    }
    delete iconImg;
    delete frameIcon;
    //delete renderImg;
}
StickFigure* Frame::addStickFigure(QListWidget * myListWidget, QString name)
{
    QString myName;
    ListWidget = myListWidget;
    if(name.count()== 0){
        int intName = 0;
        if(!stickFigures.isEmpty()){
            intName = ListWidget->currentRow()+1;
        }

        myName.sprintf("%d",intName);
    }
    else{
        myName = name;
    }
    //deseleziona graficamente nel viewport

    QListWidgetItem * addedItem = new QListWidgetItem();

    //StickFigure* addedStickFigure = view->addStickFigure( addedItem);


    if(!stickFigures.isEmpty())
        currentStickFigure->highlight(false); //de highlight old selection
    stickFigureBuffer = new StickFigure(addedItem);
    stickFigures.append(stickFigureBuffer);
    //seleziona l'ordine z in base all'ordine di creazione
    stickFigureBuffer->baseZ = stickFigures.count();
    //aggiorna lo stickfigure corrente
    currentStickFigure = stickFigureBuffer;
    currentStickFigure->name = myName;
    if(!undoFlag){
        QVariant newData(QVariant::fromValue(currentStickFigure));
        addedItem->setData(Qt::UserRole,newData);
        addedItem->setData(Qt::DisplayRole,myName);
        addedItem->setIcon(*currentStickFigure->stickFigureIcon);
        myListWidget->insertItem(myListWidget->currentRow()+1,addedItem);
    }
    // update current stickfigure
    currentStickFigure->highlight(false); //de highlight old selection
    currentStickFigure->highlight(true); //highlight new one
    if(!undoFlag){
        // update current stick
        scene->clearSelection(); //clear scene selection
        if(!currentStickFigure->stickList.isEmpty())
            selectStick(currentStickFigure); //update selected stick
        //
        int tempIndex = myListWidget->currentRow();
        myListWidget->clearSelection();
        myListWidget->setItemSelected(addedItem, true);
        myListWidget->setCurrentRow(tempIndex+1);
    }
    currentStickFigure->scene = scene;

    return stickFigureBuffer;
}
StickFigure* Frame::removeStickFigure(StickFigure* toRemove)
{
    if(stickFigures.count() > 0)
    {
        stickFigures.removeAll(toRemove);
        delete toRemove;
        if(!stickFigures.isEmpty())
            currentStickFigure = stickFigures[0];
        else
        {
            qDebug("removed all sticks");
            currentStickFigure = NULL;
        }
        stickFigureBuffer = currentStickFigure;

    }
    return currentStickFigure;
}
stick *Frame::selectStick(StickFigure* S)
{
    int stickIdx = 0;
    S->stickList[stickIdx]->setSelected(true);
    S->currentStick = S->stickList[stickIdx];
    return S->stickList[stickIdx];
}
stick *Frame::selectStick(QPointF point)
{
    float distBuffer;
    float minDist;
    int idxBuffer;
    int selectedIdx = -1;
    StickFigure *selectedStickFigure;
    for(StickFigure * S: stickFigures)
    {
        // su un criterio di distanza dall'handle utile, scegli l'item stick più vicino per ogni stickfigure
            idxBuffer = S->selectStick( &point);
            distBuffer = QLineF(S->stickList[idxBuffer]->myLine.p2(),point).length();
            //scegli il più vicino tra tutti gli stickfigure
            if(selectedIdx == -1 || (distBuffer<minDist))
            {
                selectedIdx = idxBuffer;
                minDist = distBuffer;
                selectedStickFigure = S;
            }
    }
    selectedStickFigure->stickList[selectedIdx]->setSelected(true);
    return selectedStickFigure->stickList[selectedIdx];
}
QDataStream & operator<< (QDataStream& stream, const Frame& myFrame){
    int num = myFrame.stickFigures.count();
    stream<< num ;
    stream<< myFrame.frameNumber;
    for(StickFigure* S: myFrame.stickFigures){
        stream<< *S;
    }
    return stream;
}
QDataStream & operator>> (QDataStream& stream, Frame& myFrame){
    int max = 0;
    stream>> max;
    stream>> myFrame.frameNumber;

    for(int i = 0;i<max;i++){
        StickFigure* S = myFrame.addStickFigure(ListWidget);
        stream>>*S;
        //myFrame.stickFigures.append(S);
    }
    return stream;
}

// questa funzione aggiorna l'iconea del livello
void Frame::updateIcon()
{
    //crea una scena fittizia, falla bianca e delle stesse dimensioni di quella principale
    QGraphicsScene icoScene;
    iconImg->fill(Qt::white);
    icoScene.setBackgroundBrush(QBrush(QColor(Qt::white)));
    icoScene.setSceneRect(scene->sceneRect());
    // ora clona tutti gli stick nello stickfigure in una tempList, aggiungili alla scena fittizia
    QList<stick*> tempList;
    for(StickFigure *S:stickFigures)
    {
        for(stick* s: S->stickList){
            stick* clone = new stick(s);
            tempList.append(clone);
            icoScene.addItem(clone);
        }
    }
    // crea un painter che disegni sull'immagine
    QPainter painter(iconImg);
    // renderizza dalla scena sull'immagine mediante il painter
    icoScene.render(&painter);
    // imposta l'iconea
    *frameIcon = QIcon(*iconImg);
    linkedItem->setIcon(*frameIcon);

    // cancella gli elementi costruiti
    for(stick* S: tempList){
        delete S;
    }
}
void Frame::updateRender(){
    /*QGraphicsScene renderScene;
    renderScene.setSceneRect(scene->sceneRect());
    // ora clona tutti gli stick nello stickfigure in una tempList, aggiungili alla scena fittizia
    QList<stick*> tempList;
    for(StickFigure *S:stickFigures)
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
    renderScene.clear();*/
}
void Frame::clearFrame(){
    for(StickFigure *S:stickFigures){

        delete S;

    }
    stickFigures.clear();

}
