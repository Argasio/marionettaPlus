#include "animation.h"

#include <QBuffer>
#include <QFile>
extern QListWidget * myStickFigureWidgetList;
extern QListWidget * myFrameWidgetList;
bool loadingAnimationFlag = false;
Animation::Animation()
{

}

Frame *Animation::addFrame(QListWidgetItem* item, int pos)
{

    frameBuffer = new Frame();
    frameBuffer->scene = scene;
    //frameBuffer->renderImg = new QImage(scene->sceneRect().width(),scene->sceneRect().height(),QImage::Format_ARGB32);

    if(frameList.isEmpty())
        currentFrame = frameBuffer;
    frameList.append(frameBuffer);
    frameBuffer->linkedItem = item;

}
Frame *Animation::deleteFrame(Frame *frame){
    int frameNum = frameList.indexOf(frame);
    if(frameList.count()>=1){
        frameList.removeOne(frame);
        for(StickFigure * S:frame->stickFigures){
            delete S->linkedItem;
        }
        delete frame->linkedItem;
        delete frame;
        scene->clear(); // risky
        if(!frameList.isEmpty()){
            if(frameNum>=frameList.count())
            {
                currentFrame = frameList[frameList.count()-1];
            }
            else{
                currentFrame = frameList[frameNum];
            }
        }
        else{
            currentFrame = nullptr;
            frameBuffer = nullptr;
        }
    }
    return currentFrame;
}
void Animation::updateSelection(QPointF point)
{
    float distBuffer;
    float minDist;
    int idxBuffer;
    int selectedIdx = -1;
    scene->clearSelection();
    StickFigure *selectedStickFigure;
    currentFrame->currentStickFigure->highlight(false);
    if(!currentFrame->currentStickFigure->stickList.isEmpty())
    {
        for(StickFigure * S: currentFrame->stickFigures)
        {
            if(S->stickList.count()>=1) {
                idxBuffer = S->selectStick( &point);
                // aggiorna il buffer col punto 1 o 2 a seconda se stiamo selezionando lo stick master
                if(S->selectingOrigin )
                    distBuffer = QLineF(S->stickList[idxBuffer]->myLine.p1(),point).length();
                else{
                    distBuffer = QLineF(S->stickList[idxBuffer]->myLine.p2(),point).length();
                }
                if(selectedIdx == -1 || (distBuffer<minDist))
                {
                    selectedIdx = idxBuffer;
                    minDist = distBuffer;
                    selectedStickFigure = S;
                }
            }
        }

        currentFrame->currentStickFigure->currentStick = selectedStickFigure->stickList[selectedIdx];
        selectedStickFigure->stickList[selectedIdx]->setSelected(true);
        selectedStickFigure->highlight(true);
        currentFrame->currentStickFigure = selectedStickFigure;
    }

}
/*
void Animation::storeUndo()
{
    undoBuffer.append(*currentFrame);
    if(undoBuffer.count()>25)
    {
        undoBuffer.removeFirst();
    }
}
void Animation::redo()
{
    if(redoBuffer.count()>=2){
        storeUndo();
        *currentFrame = redoBuffer.first();
        redoBuffer.removeFirst();
        currentFrame->currentStickFigure->refresh();
    }
}
void Animation::undo(){
    if(undoBuffer.count()>=2){
        redoBuffer.append(undoBuffer.last());
        undoBuffer.removeLast();
        *currentFrame = undoBuffer.last();
        currentFrame->currentStickFigure->refresh();
    }
}*/
void Animation::updateSelection(stick* item)
{
    //update current frame

    // update current stickfigure
    scene->clearSelection();
    currentFrame->currentStickFigure->highlight(false);
    for(StickFigure *S : currentFrame->stickFigures)
    {
        if(S->stickList.contains(item))
        {
            currentFrame->currentStickFigure = S;
            S->highlight(true);
        }
    }
    // update current stick
    if(!currentFrame->currentStickFigure->stickList.isEmpty())
    {
     currentFrame->currentStickFigure->currentStick = item;
     item->setSelected(true);
    }
    //int idx = frameBuffer->currentStickFigure->itemList.indexOf(item);

}
void Animation::updateSelection(StickFigure* item)
{
    //update current frame

    // update current stickfigure
    currentFrame->currentStickFigure->highlight(false); //de highlight old selection
    currentFrame->currentStickFigure = item; //update currentStickFigure buffer
    item->highlight(true); //highlight new one

    // update current stick
    scene->clearSelection(); //clear scene selection
    if(!item->stickList.isEmpty())
        currentFrame->selectStick(item); //update selected stick
}
Frame* Animation::setupFrame(int pos){
    QString name;
    name.sprintf("%d",pos);

    // prepara il widget associato
    QListWidgetItem * addedItem = new QListWidgetItem();

    for(Frame* f: frameList){
        if(f->frameNumber>=pos){
            f->frameNumber++;
            f->linkedItem->setData(Qt::DisplayRole,QString::number(f->frameNumber));
        }
    }
    myFrameWidgetList->insertItem(pos,addedItem);
    Frame * addedFrame = addFrame(addedItem, pos);
    addedFrame->frameNumber = pos;
    addedFrame->linkedItem = addedItem;
    // popola l'entry QListWidget con puntatore ed iconea del frame
    QVariant newData(QVariant::fromValue(addedFrame));
    addedItem->setData(Qt::UserRole,newData);
    addedItem->setData(Qt::DisplayRole,name);
    addedItem->setIcon(*addedFrame->frameIcon);
    // update other frames
    return addedFrame;
}
void Animation::cloneFrame(Frame* target, Frame* source)
{
    // byte array stores serialized data
    QByteArray byteArray;
    // buffer temporarily holds serialized data
    QBuffer buffer1(&byteArray);
    // use this buffer to store data from the object
    buffer1.open(QIODevice::WriteOnly);
    QDataStream myStream(&buffer1);
    myStream<<*(source);
    // now create a seconds buffer from which to read data of the bytearray
    QBuffer buffer2(&byteArray);
    buffer2.open(QIODevice::ReadOnly);
    // a new data stream to deserialize
    QDataStream myStream2(&buffer2);
    // hydrate new frame with previous frame data
    myStream2>>*target;
    buffer1.close();
    buffer2.close();

    //target->linkedItem = source->linkedItem;
    //target->scene = source->scene;
}
QDataStream &operator<<(QDataStream& stream, const Animation& myAnimation){
    int frameNum = myAnimation.frameList.count();
    stream<<frameNum;
    for(Frame* f: myAnimation.frameList){
        stream<<*f;
    }
}
QDataStream &operator>>(QDataStream& stream,Animation& myAnimation){
    int frameNum = 0;
    stream>>frameNum;
    for(int i = 0;i<frameNum;i++){
        Frame* addedFrame = myAnimation.setupFrame(i);
        stream>>*addedFrame;
        addedFrame->updateIcon();
    }

}
void Animation::saveAnimation(QString fileName){
    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly))
        return;
    QDataStream out(&file);
    out << *this;
    file.close();
}
void Animation::loadAnimation(QString name)
{

    QFile file(name);
    if(!file.open(QIODevice::ReadOnly))
        return;
    QDataStream out(&file);
    clearAnimation();
    loadingAnimationFlag = true;
    out >> *this;
    loadingAnimationFlag = false;
    file.close();
    qDebug("load finished");

}
void Animation::clearAnimation(){
    for(Frame* f: frameList){
        deleteFrame(f);
    }

}
