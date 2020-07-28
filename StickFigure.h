#ifndef StickFigure_H
#define StickFigure_H
#include <QPointF>
#include <QLineF>
#include <QtMath>
#include <stick.h>
#include <QGraphicsScene>
#include <QListWidgetItem>

class StickFigure
{
public:
    ~StickFigure();
    StickFigure(QListWidgetItem* item);
    void  startDrawing(QPointF* point);
    void  previewDrawing(QPointF* point);
    void  endDrawing(QPointF* point);
    void  returnLine();
    void  cancelDrawing();
    void deleteStick(int idx);
    int drawCount;
    int selectStick( QPointF * point);
    QList<stick*>stickList;
    void setLineFromPoint(QPointF *point);
    QLineF lineBuffer;
    stick* currentStick;
    stick* masterStick;
    qreal baseZ;
    void highlight(bool setting);
    QListWidgetItem* linkedItem;
    QGraphicsScene * scene;
    void updateZ();
    QIcon*stickFigureIcon;
    QPixmap * iconImg;
    void updateIcon();
    void copyData(StickFigure *toCopy);
    void serialize(QDataStream *myStream);
    void deSerialize(QDataStream *myStream);

private:

    QPointF p0;
    QPointF p1;
    stick* stickBuffer;
    QPointF selectOrigin( QPointF * point);
    stick* parentBuffer = NULL;
    QGraphicsScene * myScene ;
    bool selectingOrigin = false;

};
QDataStream &operator<<(QDataStream & myStream, const StickFigure* myStickFigure);
QDataStream &operator>>(QDataStream & myStream,  StickFigure* myStickFigure);
QDataStream &operator>>(QDataStream & myStream, const QList<stick*> mySticks);
QDataStream &operator<<(QDataStream & myStream, const QList<stick*>& mySticks);
QDataStream &operator>>(QDataStream & myStream, stick& myStick);
QDataStream &operator<<(QDataStream & myStream, const stick& myStick);

// to use QVariant in order to insert a stickfigure pointer into the widget list
Q_DECLARE_METATYPE(StickFigure*)
#endif // StickFigure_H
