#include "widget.h"
#include "ui_widget.h"
#include <QGraphicsRectItem>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <frame.h>
#include <animation.h>
#include <QPen>
#include <QIcon>
#include <QVariant>
#include <QDebug>
#include <QColorDialog>
#include <QBuffer>
QGraphicsScene *scene;

QList <StickFigure*> layerList;
QListWidget * myStickFigureWidgetList;
QListWidget * myFrameWidgetList;
QSpinBox * onionSkinSB;
QTextEdit * stickFigureNameText;
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{

    ui->setupUi(this);
    //crea la scena virtuale
    myStickFigureWidgetList = ui->stickLayerView;
    myFrameWidgetList = ui->frameListWidget;
    onionSkinSB = ui->onionSkinSpinBox;
    stickFigureNameText = ui->stickFigureNameText;
    scene = new QGraphicsScene(this);
    //crea il pannello e collegalo alla scena
    view = new myView(this);
    view->setScene(scene);
    view->myAnimation->scene = scene;
    addFrame();
    //inserisci il pannello nella finestra
    ui->viewLayout->addWidget(view);
    //crea la cornice
    QGraphicsRectItem *item = new QGraphicsRectItem(0, 0, 600, 600);
    scene->addItem(item);


}

Widget::~Widget()
{
    delete ui;
}


void Widget::on_drawBtn_clicked()
{
    view->setTool(DRAW);
}

void Widget::on_cursor_clicked()
{
    view->setTool(NOTOOL);
}

void Widget::on_addStickBtn_clicked()
{
    view->storeUndo();
    addStick();
}

void Widget::addStick(){
    view->myAnimation->currentFrame->addStickFigure(ui->stickLayerView, stickFigureNameText->toPlainText());
    stickFigureNameText->clear();
}
void Widget::on_thicknessSpinBox_valueChanged(int arg1)
{
    view->myPen.setWidth(arg1);
}

void Widget::on_stickLayerView_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    if(current != nullptr)
    {
        qDebug("layer clicked");
        QVariant  retrievedData = (current->data(Qt::UserRole));
        StickFigure* cs = qvariant_cast<StickFigure*>(retrievedData);
        view->myAnimation->updateSelection(cs);
    }
}

void Widget::on_stickLayerView_itemClicked(QListWidgetItem *item)
{

}

void Widget::on_deleteStickFigureBtn_clicked()
{
    view->deleteStickFigure();
}

void Widget::on_moveLayerUpBtn_clicked()
{
    view->moveStickFigureZ(1);
}

void Widget::on_moveLayerDownBtn_clicked()
{
    view->moveStickFigureZ(-1);
}

void Widget::on_drawCircleBtn_clicked()
{
    view->setTool(DRAWCIRCLE);
}

void Widget::on_chooseColorBtn_clicked()
{
    QColor color = QColorDialog::getColor(Qt::yellow, this );
    view->myPen.setColor(color);
    view->myBrush.setColor(color);
    // aggiorna il colore del segnacolore
    if(color.isValid()) {
       QString qss = QString("background-color: %1").arg(color.name());
       ui->currentColorBtn->setStyleSheet(qss);
    }
}

void Widget::on_undoBtn_clicked()
{
    view->undoRedo(MODE_UNDO);
}

void Widget::on_redoBtn_clicked()
{
    view->undoRedo(MODE_REDO);
}
void Widget::on_addFrameBtn_clicked()
{
    addFrame();
}
void Widget::addFrame(void)
{
    view->storeUndo(CMD_ADDFRAME);
    int pos = 0;
    if(view->myAnimation->frameList.count()>=1)
        pos = view->myAnimation->currentFrame->frameNumber+1;
    Frame* addedFrame = setUpFrame(pos);
    view->moveToFrame(addedFrame);
    addStick();
    //view->myAnimation->storeUndo();
}

void Widget::on_frameListWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{

}
// questa funzione crea il frame e gli oggetti lista appropriati
Frame* Widget::setUpFrame(int pos){
    return view->setUpFrame(pos);

}
void Widget::on_copyFrame_clicked()
{
    view->storeUndo(CMD_ADDFRAME);
    // byte array stores serialized data
    QByteArray byteArray;
    // buffer temporarily holds serialized data
    QBuffer buffer1(&byteArray);
    // use this buffer to store data from the object
    buffer1.open(QIODevice::WriteOnly);
    QDataStream myStream(&buffer1);
    myStream<<*(view->myAnimation->currentFrame);
    // now create a seconds buffer from which to read data of the bytearray
    QBuffer buffer2(&byteArray);
    buffer2.open(QIODevice::ReadOnly);
    // a new data stream to deserialize
    QDataStream myStream2(&buffer2);
    // prepare new frame
    int pos = 0;
    if(view->myAnimation->frameList.count()>=1)
        pos = view->myAnimation->currentFrame->frameNumber+1;
    Frame* addedFrame = setUpFrame(pos);
    // hydrate new frame with previous frame data
    myStream2>>*addedFrame;
    // update frame name
    addedFrame->frameNumber++;
    // move to new frame
    view->moveToFrame(addedFrame);
    buffer1.close();
    buffer2.close();
    // update list icons

}
void Widget::copyFrame(QDataStream& stream, Frame* destination)
{



}

void Widget::on_onionSkinSpinBox_valueChanged(const QString &arg1)
{
    view->updateOnionSkins();
}

void Widget::on_deleteFrameBtn_clicked()
{
    if(view->myAnimation->frameList.count()>1){
        view->storeUndo(CMD_DELETEFRAME);
        view->deleteFrame(view->myAnimation->currentFrame);
        view->moveToFrame(view->myAnimation->currentFrame);
    }
}

void Widget::on_frameListWidget_itemClicked(QListWidgetItem *item)
{
    view->storeUndo(CMD_MOVETOFRAME);
    QVariant  retrievedData = (item->data(Qt::UserRole));
    Frame* cs = qvariant_cast<Frame*>(retrievedData);

    view->moveToFrame(cs);
}
