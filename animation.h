#ifndef ANIMATION_H
#define ANIMATION_H
#include <frame.h>
#include <QGraphicsItem>
#include <QListWidgetItem>
class Animation
{
public:
    Animation();
    QList<Frame*> frameList;
    Frame *frameBuffer;
    Frame * addFrame(QListWidgetItem* item);
    Frame* currentFrame;
    void updateSelection(stick* item);
    void updateSelection(StickFigure* item);
    void updateSelection(QPointF point);
    QGraphicsScene * scene;
    QList <Frame> undoBuffer;
    QList <Frame> redoBuffer;
    void storeUndo();
    void redo();
    void undo();
};

#endif // ANIMATION_H
