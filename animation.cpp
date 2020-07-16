#include "animation.h"

Animation::Animation()
{
    frameBuffer = new Frame();
    currentFrame = frameBuffer;
    frameList.append(frameBuffer);
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
                idxBuffer = S->selectStick( &point);
                distBuffer = QLineF(S->stickList[idxBuffer]->myLine.p2(),point).length();
                if(selectedIdx == -1 || (distBuffer<minDist))
                {
                    selectedIdx = idxBuffer;
                    minDist = distBuffer;
                    selectedStickFigure = S;
                }
        }

        currentFrame->currentStickFigure->currentStick = selectedStickFigure->stickList[selectedIdx];
        selectedStickFigure->stickList[selectedIdx]->setSelected(true);
        selectedStickFigure->highlight(true);
        currentFrame->currentStickFigure = selectedStickFigure;
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

    //int idx = frameBuffer->currentStickFigure->itemList.indexOf(item);

}
