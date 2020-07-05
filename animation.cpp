#include "animation.h"

Animation::Animation()
{
    frameBuffer = new Frame();
    currentFrame = frameBuffer;
    frameList.append(frameBuffer);
}
void Animation::updateSelection(QGraphicsLineItem* item)
{
    //update current frame

    // update current stickfigure
    for(StickFigure *S : currentFrame->stickFigures)
    {
        if(S->itemList.contains(item))
        {
            currentFrame->currentStickFigure = S;
            break;
        }
    }
    // update current stick

    int idx = frameBuffer->currentStickFigure->itemList.indexOf(item);
    currentFrame->currentStickFigure->currentStick = currentFrame->currentStickFigure->stickList[idx];
}
