#include "animation.h"

Animation::Animation()
{

}

Frame *Animation::addFrame(QListWidgetItem* item)
{

    frameBuffer = new Frame();
    frameBuffer->scene = scene;
    //frameBuffer->renderImg = new QImage(scene->sceneRect().width(),scene->sceneRect().height(),QImage::Format_ARGB32);

    if(frameList.isEmpty())
        currentFrame = frameBuffer;
    frameList.append(frameBuffer);
    frameBuffer->linkedItem = item;

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
void Animation::storeUndo()
{/*
    undoBuffer.append(*currentFrame);
    if(undoBuffer.count()>25)
    {
        undoBuffer.removeFirst();
    }*/
}
void Animation::redo()
{/*
    if(redoBuffer.count()>=2){
        storeUndo();
        *currentFrame = redoBuffer.first();
        redoBuffer.removeFirst();
        currentFrame->currentStickFigure->refresh();
    }*/
}
void Animation::undo(){
    if(undoBuffer.count()>=2){
        redoBuffer.append(undoBuffer.last());
        undoBuffer.removeLast();
        *currentFrame = undoBuffer.last();
        currentFrame->currentStickFigure->refresh();
    }
}
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
