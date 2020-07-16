#include "frame.h"
#include <QListWidget>
#include <QDebug>
Frame::Frame()
{
}
StickFigure* Frame::addStickFigure(QListWidgetItem* item)
{
    if(!stickFigures.isEmpty())
        currentStickFigure->highlight(false); //de highlight old selection
    stickFigureBuffer = new StickFigure(item);
    stickFigures.append(stickFigureBuffer);
    stickFigureBuffer->baseZ = stickFigures.count();
    currentStickFigure = stickFigureBuffer;
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
            //currentStickFigure = NULL;
        }
        stickFigureBuffer = currentStickFigure;
        return currentStickFigure;
    }
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
            idxBuffer = S->selectStick( &point);
            distBuffer = QLineF(S->stickList[idxBuffer]->myLine.p2(),point).length();
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
