#include "widget.h"
#include "ui_widget.h"
#include "sleeper.h"
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
#include <QInputDialog>
#include <QThread>
#include <QTimer>
#include <QProcess>
#include <QDir>
#include <QStandardPaths>
#include "QAviWriter.h"
//#include "QVideoEncoder.h"
//#include "QVideoDecoder.h"
QGraphicsScene *scene;

QList <StickFigure*> layerList;
QListWidget * myStickFigureWidgetList;
QListWidget * myFrameWidgetList;
QSpinBox * onionSkinSB;
QTextEdit * stickFigureNameText;
QGraphicsRectItem *myRect;
QGraphicsRectItem *limitRect;
QSpinBox* fpsSpinBox;
QPointF onionOffset;
QDir programFolder;
int W = 600;
int H = 600;
bool playBack = false;
float zoomLvl = 1;
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{

    ui->setupUi(this);
    //crea la scena virtuale
    QString  path = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    programFolder = QDir(path+"/MarionettaPlus");
    if(programFolder.exists()){
        qDebug("folder located");
    }
    else{
        qDebug("folder created");
        programFolder.mkpath(path+"/MarionettaPlus");
    }
    myStickFigureWidgetList = ui->stickLayerView;
    myFrameWidgetList = ui->frameListWidget;
    onionSkinSB = ui->onionSkinSpinBox;
    stickFigureNameText = ui->stickFigureNameText;
    fpsSpinBox = ui->fpsSpinBox;
    scene = new QGraphicsScene(this);
    //crea il pannello e collegalo alla scena
    view = new myView(this);
    view->setScene(scene);
    view->myAnimation->scene = scene;
    addFrame();
    //inserisci il pannello nella finestra
    ui->viewLayout->addWidget(view);
    //crea la cornice
    myRect = new QGraphicsRectItem(0, 0, 600, 600);
    limitRect = new QGraphicsRectItem(-myRect->rect().width()/10, -myRect->rect().height()/10, myRect->rect().width()*1.1, myRect->rect().height()*1.1);
    scene->addItem(myRect);
    scene->setSceneRect(limitRect->rect());
    onionOffset = QPointF(-myRect->rect().width()/10,-myRect->rect().height()/10);

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

void Widget::on_frameListWidget_itemDoubleClicked(QListWidgetItem *item)
{

}

void Widget::on_stickLayerView_itemDoubleClicked(QListWidgetItem *item)
{
    QString text = QInputDialog::getText(this,"rename layer","text");
    QListWidgetItem *myItem = myStickFigureWidgetList->item((myStickFigureWidgetList->currentRow()));
    myItem->setData(Qt::DisplayRole, text);
    QVariant  retrievedData = (item->data(Qt::UserRole));
    StickFigure* S = qvariant_cast<StickFigure*>(retrievedData);
    S->name = text;
}

void Widget::on_PlayButton_clicked()
{
    view->moveToFrame(view->myAnimation->frameList[0]);
    int fps = fpsSpinBox->value();
    int onions = onionSkinSB->value();

    onionSkinSB->setValue(0);
    view->moveToFrame(view->myAnimation->frameList[0]);
    playBack = true;
    int delay = 1000/fps;
    if(delay>5)
        delay-=5;
    mySleeper *play = new mySleeper(view,delay);
    onionSkinSB->setValue(onions);
    playBack = false;
    view->moveToFrame(view->myAnimation->frameList[0]);

}

void Widget::on_zoomInBtn_clicked()
{
    view->scale(1/zoomLvl,1/zoomLvl);
    zoomLvl*=2;
    view->scale(zoomLvl,zoomLvl);
    view->moveToFrame(view->myAnimation->currentFrame);
}

void Widget::on_zoomOutBtn_clicked()
{

    view->scale(1/zoomLvl,1/zoomLvl);
    zoomLvl/=2;
    view->scale(zoomLvl,zoomLvl);
    view->moveToFrame(view->myAnimation->currentFrame);
}

void Widget::on_resetZoomBtn_clicked()
{
    view->scale(1/zoomLvl,1/zoomLvl);
    zoomLvl = 1.0;
    view->moveToFrame(view->myAnimation->currentFrame);
}

void Widget::on_exportBtn_clicked()
{
    //QVideoEncoder encoder;
    /*
    QProcess *p = new QProcess(this);
    QStringList paramList;
    paramList<<"-i"<<"C:/Users/Argasio/Documents/GitHub/MarionettaPlus/MarionettaPlus/input.mp4"
            <<"C:/Users/Argasio/Documents/GitHub/MarionettaPlus/MarionettaPlus/output.webm";
    p->startDetached("C:/ffmpeg-20200814-a762fd2-win64-static/bin/ffmpeg.exe",paramList);*/
    QAviWriter writer("C:/Users/Argasio/Documents/GitHub/MarionettaPlus/demo.avi", QSize(W, H), 24, "MJPG");// set framerate to 24 fps and 'MJPG' codec
    //writer.setAudioFileName("audio.wav"); // set audio track
    writer.open();
    for(Frame* f :view->myAnimation->frameList){


        QGraphicsScene renderScene;
        QImage *renderImg = new QImage(scene->sceneRect().width(),scene->sceneRect().height(),QImage::Format_ARGB32);

        renderScene.setSceneRect(scene->sceneRect());
        // ora clona tutti gli stick nello stickfigure in una tempList, aggiungili alla scena fittizia
        QList<stick*> tempList;
        for(StickFigure *S:f->stickFigures)
        {
            for(stick* s: S->stickList){
                stick* clone = new stick(s);
                tempList.append(clone);
                renderScene.addItem(clone);
            }
        }
        QPainter painter(renderImg);
        renderImg->fill(Qt::transparent);
        painter.setBackground(QBrush(QColor(Qt::transparent)));
        renderScene.render(&painter);
        writer.addFrame(*renderImg);
        delete renderImg;
        renderScene.clear();
        //...add all other video frames here
    }
    writer.close();
}
