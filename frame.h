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
};

#endif // FRAME_H
