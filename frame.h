#ifndef FRAME_H
#define FRAME_H
#include <StickFigure.h>
#include <QGraphicsScene>
class Frame
{
public:
    Frame();
    StickFigure* addStickFigure(QListWidgetItem* item);
    QList<StickFigure*> stickFigures;
    StickFigure* stickFigureBuffer;
    StickFigure* currentStickFigure;
    stick *selectStick(QPointF point);
    stick *selectStick(StickFigure* S);
    StickFigure* removeStickFigure(StickFigure* toRemove);
    QIcon* frameIcon;
};

#endif // FRAME_H
