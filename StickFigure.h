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
    bool selectingOrigin = false;
private:

    QPointF p0;
    QPointF p1;
    stick* stickBuffer;
    QPointF selectOrigin( QPointF * point);
    stick* parentBuffer = nullptr;
    QGraphicsScene * myScene ;


};
// to use QVariant in order to insert a stickfigure pointer into the widget list
Q_DECLARE_METATYPE(StickFigure*)
#endif // StickFigure_H
