#ifndef MYVIEW_H
#define MYVIEW_H


#include <QGraphicsView>
#include <QPointF>
#include <QMouseEvent>
#include <QGraphicsItem>
#include <QLineF>
#include <StickFigure.h>
#include <animation.h>
#define DRAW 5
#define DRAWCIRCLE 6
#define DRAWIMG 7
#define NOTOOL 4


#define CMD_SIMPLE 0
#define CMD_ADDFRAME 1
#define CMD_DELETEFRAME 2
#define CMD_MOVETOFRAME 3

#define MODE_REDO 0
#define MODE_UNDO 1
typedef struct undoInfoStruct
{
    int command;
    Frame* frame;
};

class myView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit myView(QWidget *parent = nullptr);
    unsigned int tool = NOTOOL;
    void setTool(int Tool);
    StickFigure *addStickFigure(QListWidgetItem* item);
    Animation * myAnimation;
    void deleteStickFigure();
    void moveStickFigureZ(int increment);
    QPen    myPen;
    QBrush  myBrush;
    void moveToFrame(Frame *frame);
    void updateOnionSkins();
    QList<QGraphicsPixmapItem*> onionSkins;
    QList <undoInfoStruct> undoBuffer;
    QList <undoInfoStruct> redoBuffer;
    void undoRedo(int mode);
    void storeUndo(int command = CMD_SIMPLE, int mode = MODE_UNDO);
    void deleteFrame(Frame *frame);
    Frame *setUpFrame(int pos);
    void storeRedo(int command = CMD_SIMPLE);
    void mapMyCoords(QPointF &myCoord, QPointF startingPoint);
    void clearUndo();
signals:


    // QWidget interface
protected:
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void keyPressEvent(QKeyEvent *event) override;
private:
    void drawCmd(QPointF* point, int mode);
    void rotate(QGraphicsLineItem* item);
    void updateRotationBuffer();
    void changeTool();

    // QWidget interface


};

#endif // MYVIEW_H
