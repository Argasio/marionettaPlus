#ifndef FRAME_H
#define FRAME_H
#include <StickFigure.h>
#include <QGraphicsScene>
class Frame
{
public:
    Frame();
    ~Frame();
    //StickFigure* addStickFigure(QListWidgetItem* item);
    QList<StickFigure*> stickFigures;
    StickFigure* stickFigureBuffer;
    StickFigure* currentStickFigure;
    stick *selectStick(QPointF point);
    stick *selectStick(StickFigure* S);
    StickFigure* removeStickFigure(StickFigure* toRemove);
    QIcon* frameIcon;
    QPixmap * iconImg;
    //QImage * renderImg;
    QGraphicsScene * scene;
    int frameNumber = 0;
    QListWidgetItem * linkedItem;
    void updateIcon();

    void copyFrame(QDataStream &stream, Frame *destination);
    StickFigure *addStickFigure( QListWidget *myListWidget);
    void updateRender();
    void clearFrame();
    void cloneFrame(Frame *target, Frame *source);
};
Q_DECLARE_METATYPE(Frame*);
QDataStream & operator<< (QDataStream& stream, const Frame& myFrame);
QDataStream & operator>> (QDataStream& stream, Frame& myFrame);
#endif // FRAME_H
