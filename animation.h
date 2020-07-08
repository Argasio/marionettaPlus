#ifndef ANIMATION_H
#define ANIMATION_H
#include <frame.h>
#include <QGraphicsItem>
class Animation
{
public:
    Animation();
    QList<Frame*> frameList;
    Frame *frameBuffer;
    Frame * addFrame();
    Frame* currentFrame;
    void updateSelection(stick* item);
    void updateSelection(StickFigure* item);
};

#endif // ANIMATION_H