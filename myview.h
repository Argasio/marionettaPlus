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
#define MANIPULATE 8
#define ROTATE 9
#define SCALE 10
#define NOTOOL 4
#define MOVESCENE 11
#define ROTATESCENE 12
#define SCALESCENE 13
#define JOIN 14
#define SPLIT 15
#define DRAWRECT 16
#define DRAWTRAPEZOID 17
#define DRAWTAPER 18

#define CMD_SIMPLE 0
#define CMD_ADDFRAME 1
#define CMD_DELETEFRAME 2
#define CMD_MOVETOFRAME 3

#define MODE_REDO 0
#define MODE_UNDO 1

#define ATTRIBUTE_PENCOLOR 0
#define ATTRIBUTE_PENWIDTH 1
#define ATTRIBUTE_BRUSHCOLOR 2
#define ATTRIBUTE_BRUSHSIZE 3
typedef struct undoInfoStruct
{
    int command;
    Frame* frame;
}undoInfoStruct;

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
    StickFigure* copyStickFigureBuffer = nullptr;
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
    void loadLibrary(QString fileName);
    StickFigure *addStickFigureToLibrary();
    QList<QGraphicsPixmapItem*> previewList;
    void removeFromLibrary();
    void setGraphics(bool all, int attribute);
    void splitStickFigure();
    void clearCurrentLib();
    void sizeChange(int option);
    void scaleFrame(Frame *F, float scaleAmount, QRectF R);
    void preparePreview();
    void displayPreview(int idx);
public slots:
    void saveLibrary(QString fileName);
signals:
    void  autoSaveTrigger();


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
