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
    int numOfItems = 0;
    QPixmap * iconImg;
    //QImage * renderImg;
    QGraphicsScene * scene;
    int frameNumber = 0;
    QListWidgetItem * linkedItem;
    void updateIcon();
    QList <stick*> totalSticks;
    void copyFrame(QDataStream &stream, Frame *destination);
    StickFigure *addStickFigure( QListWidget *myListWidget, QString name = "");
    void updateRender();
    void clearFrame();
    void normalizeDepth();
    void cloneFrame(Frame *target, Frame *source);
    StickFigure *addStickFigureToLibrary(StickFigure *S = nullptr);
    void refresh(int mode = 0);
    void stickfigureToBottom(StickFigure *S);
    void stickfigureToTop(StickFigure *S);
    float minMaxZ(int option);
    void stickfigureAboveBelow(StickFigure *toMove, StickFigure *previous, int mode);
};
Q_DECLARE_METATYPE(Frame*);
QDataStream & operator<< (QDataStream& stream, const Frame& myFrame);
QDataStream & operator>> (QDataStream& stream, Frame& myFrame);
#endif // FRAME_H
