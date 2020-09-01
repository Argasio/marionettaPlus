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
    StickFigure();
    void  startDrawing(QPointF* point, QPen pen);
    void  previewDrawing(QPointF* point);
    void  endDrawing(QPointF* point);
    void  returnLine();
    void  cancelDrawing();
    void deleteStick(int idx);
    void saveStickFigure(QString name);
    int drawCount;
    int stickNum = 0;
    int version = 1;
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

    QString name;
    void updateIcon();
    bool selectingOrigin = false;
    void loadStickFigure(QString name);
    void refresh(int mode = 0);


    void rotateStickFigure(QPointF *coord);
    void scale(QPointF *coord);
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
QDataStream &operator<<(QDataStream& stream, const StickFigure& myStickFigure);
QDataStream &operator>>(QDataStream& stream,StickFigure& myStickFigure);
void cloneStickFigure(StickFigure *dest, StickFigure *src);
#endif // StickFigure_H
