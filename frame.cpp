#include "frame.h"

Frame::Frame()
{
    addStickFigure();
}
StickFigure* Frame::addStickFigure()
{
    stickFigureBuffer = new StickFigure();
    stickFigures.append(stickFigureBuffer);
    currentStickFigure = stickFigureBuffer;
    return stickFigureBuffer;
}
