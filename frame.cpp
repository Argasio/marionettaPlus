#include "frame.h"
#include <QListWidget>
extern QListWidget *stickListView;
Frame::Frame()
{
}
StickFigure* Frame::addStickFigure()
{
    stickFigureBuffer = new StickFigure();
    stickFigureBuffer->baseZ = stickFigures.count();
    stickFigures.append(stickFigureBuffer);
    currentStickFigure = stickFigureBuffer;

    return stickFigureBuffer;
}

stick *Frame::selectStick(QPointF point)
{
    float distBuffer;
    float minDist;
    int idxBuffer;
    int selectedIdx = -1;
    StickFigure *selectedStickFigure;
    for(StickFigure * S: this->stickFigures)
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
    //stickListView->setItemSelected((QListWidgetItem*)selectedStickFigure->layerName,true);
    // stickListView->setCurrentItem((QListWidgetItem*)selectedStickFigure->layerName);
     //   stickListView->setItemSelected((QListWidgetItem*)selectedStickFigure->layerName,true);
    selectedStickFigure->stickList[selectedIdx]->setSelected(true);
    return selectedStickFigure->stickList[selectedIdx];
}
