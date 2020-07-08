#ifndef FRAME_H
#define FRAME_H
#include <StickFigure.h>
#include <QGraphicsScene>
class Frame
{
public:
    Frame();
    StickFigure* addStickFigure();
    QList<StickFigure*> stickFigures;
    StickFigure* stickFigureBuffer;
    StickFigure* currentStickFigure;
    stick *selectStick(QPointF point);
};

#endif // FRAME_H
