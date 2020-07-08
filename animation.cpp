#include "animation.h"

Animation::Animation()
{
    frameBuffer = new Frame();
    currentFrame = frameBuffer;
    frameList.append(frameBuffer);
}
void Animation::updateSelection(stick* item)
{
    //update current frame

    // update current stickfigure
    for(StickFigure *S : currentFrame->stickFigures)
    {
        if(S->stickList.contains(item))
        {
            currentFrame->currentStickFigure = S;
            break;
        }
    }
    // update current stick

    //int idx = frameBuffer->currentStickFigure->itemList.indexOf(item);

}
void Animation::updateSelection(StickFigure* item)
{
    //update current frame

    // update current stickfigure

    currentFrame->currentStickFigure = item;

    // update current stick

    //int idx = frameBuffer->currentStickFigure->itemList.indexOf(item);

}
