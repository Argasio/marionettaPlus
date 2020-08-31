#include "widget.h"
#include <sticklib.h>
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
#include <QFileDialog>
#include <QDirIterator>

//#include "QVideoEncoder.h"
//#include "QVideoDecoder.h"
QGraphicsScene *scene;

QList <StickFigure*> layerList;
QListWidget * myStickFigureWidgetList;
QListWidget * myFrameWidgetList;
QListWidget * myLibraryListWidget ;
QListWidget * myCurrentLibraryWidget;
QSpinBox * onionSkinSB;
QSpinBox * imgHOffsetSpinbox;
QSpinBox * imgVOffsetSpinbox;
QSpinBox * imgWidthSpinbox;
QSpinBox * imgHeightSpinbox;
QSpinBox * imgRotationSpinbox;
QSpinBox * penOpacitySpinbox;
QSpinBox * brushOpacitySpinbox;
QSlider * penOpacitySlider;
QSlider * brushOpacitySlider;
QTextEdit * stickFigureNameText;
QGraphicsRectItem *myRect;
QGraphicsRectItem *limitRect;
QSpinBox* fpsSpinBox;
QSlider* imgHOffsetSlider;
QSlider* imgVOffsetSlider;
QSlider* imgWidthSlider;
QSlider* imgHeightSlider;
QSlider* imgRotationSlider;
QPointF onionOffset;
QDir programFolder;
QDir libFolder;
QDir tempRenderFolder;

int W = 600;
int H = 600;
struct{
    bool sliderHOffset = false;
    bool sliderVOffset = false;
    bool sliderWScale = false;
    bool sliderHScale = false;
    bool sliderRot = false;
}sliderFlags;
bool playBack = false;
float zoomLvl = 1;
QImage * imageDrawBuffer;
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    //stickLib *myStickLib = new stickLib(this);
    ui->setupUi(this);
    //crea la scena virtuale
    //ui->tabWidget->addTab(myStickLib, "StickFigure Library");

    ui->toolsTabWidget->setCurrentWidget(ui->drawingTab);
    myStickFigureWidgetList = ui->stickLayerView;
    myLibraryListWidget = ui->libraryList;
    myCurrentLibraryWidget = ui->currentLibrary;
    myFrameWidgetList = ui->frameListWidget;
    onionSkinSB = ui->onionSkinSpinBox;
    stickFigureNameText = ui->stickFigureNameText;
    fpsSpinBox = ui->fpsSpinBox;
    imgHOffsetSlider = ui->imgHOffsetSlider;
    imgHOffsetSpinbox = ui->imgHOffsetSpinbox;
    imgVOffsetSlider = ui->imgVOfsetSlider;
    imgVOffsetSpinbox = ui->imgVOffsetSpinbox;
    imgWidthSlider = ui->imgWSlider;
    imgWidthSpinbox = ui->imgWidthSpinbox;
    imgHeightSlider = ui->imgHSlider;
    imgHeightSpinbox = ui->imgHeightSpinbox;
    imgRotationSlider = ui->imgRotationSlider;
    imgRotationSpinbox = ui->imgRotationSpinbox;
    penOpacitySlider = ui->penOpacitySlider;
    penOpacitySpinbox = ui->penOpacitySpinbox;
    brushOpacitySlider = ui->fillOpacitySlider;
    brushOpacitySpinbox = ui->fillOpacitySpinbox;
    createPaths();
    detectLibraries();
    // aggiorna il colore del segnacolore
    QColor color(Qt::black);
    if(color.isValid()) {
       QString qss = QString("background-color: %1").arg(color.name());
       ui->currentColorBtn->setStyleSheet(qss);
    }
    QColor colorBrush(Qt::black);
    if(color.isValid()) {
       QString qss = QString("background-color: %1").arg(colorBrush.name());
       ui->currentBrushColorBtn->setStyleSheet(qss);
    }
    scene = new QGraphicsScene(this);
    //crea il pannello e collegalo alla scena
    view = new myView(this);
    view->setScene(scene);
    view->myAnimation->scene = scene;
    addFrame();
    //inserisci il pannello nella finestra
    ui->viewLayout->addWidget(view);
    //crea la cornice
    myRect = new QGraphicsRectItem(0, 0, W, H);
    limitRect = new QGraphicsRectItem(-myRect->rect().width()/10, -myRect->rect().height()/10, myRect->rect().width()*1.1, myRect->rect().height()*1.1);
    scene->addItem(myRect);
    scene->setSceneRect(limitRect->rect());
    // adjust image editing offset sliders according to max size of scene
    imgVOffsetSlider->setRange(-myRect->rect().height(), myRect->rect().height());
    imgVOffsetSpinbox->setRange(-myRect->rect().height(), myRect->rect().height());
    imgHOffsetSlider->setRange(-myRect->rect().width(), myRect->rect().width());
    imgHOffsetSpinbox->setRange(-myRect->rect().width(), myRect->rect().width());
    // set onion rendered image skin offset to be displayed properly on the scene
    onionOffset = QPointF(-myRect->rect().width()/10,-myRect->rect().height()/10);
    //check if default library exists
    if(!QFile::exists(libFolder.path()+"/defaultLib.marlib")){
        view->saveLibrary(libFolder.path()+"/defaultLib.marlib");
    }
    else{
        view->loadLibrary(libFolder.path()+"/defaultLib.marlib");
    }
    myLibraryListWidget->clear();
    detectLibraries();
}
void Widget::createPaths(){
    QString  path = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    programFolder = QDir(path+"/MarionettaPlus");
    if(programFolder.exists()){
        qDebug("folder located");
    }
    else{
        qDebug("folder created");
        programFolder.mkpath(path+"/MarionettaPlus");
    }
    libFolder = QDir(path+"/MarionettaPlus/Libraries");
    if(!libFolder.exists()){
        qDebug("library folder created");
        programFolder.mkpath(path+"/MarionettaPlus/Libraries");
    }
    tempRenderFolder = QDir(path+"/MarionettaPlus/temporaryRender");
    if(!tempRenderFolder.exists()){
        qDebug("temporary render folder created");
        programFolder.mkpath(path+"/MarionettaPlus/temporaryRender");
    }

}
void Widget::detectLibraries(){
    QDirIterator dirIt(libFolder.path(),QDir::AllEntries);
    while (dirIt.hasNext()) {
        dirIt.next();
        if (QFileInfo(dirIt.filePath()).isFile()){
            if (QFileInfo(dirIt.filePath()).suffix() == "marlib"){
                QListWidgetItem * item = new QListWidgetItem(myLibraryListWidget);
                QVariant newData(QVariant::fromValue(dirIt.filePath()));
                item->setData(Qt::UserRole,newData);
                item->setData(Qt::DisplayRole,dirIt.fileName());
                myLibraryListWidget->addItem(item);
            }
        }
    }
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

StickFigure* Widget::addStick(){
    StickFigure * added = view->myAnimation->currentFrame->addStickFigure(ui->stickLayerView, stickFigureNameText->toPlainText());
    stickFigureNameText->clear();
    return added;
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
    //parti dal frame 0, scegli il framerate
    view->moveToFrame(view->myAnimation->frameList[0]);
    int fps = fpsSpinBox->value();
    int onions = onionSkinSB->value();
    // non voglio onionskins in questa modalità
    onionSkinSB->setValue(0);
    view->moveToFrame(view->myAnimation->frameList[0]);
    playBack = true;//flag globale
    // calcola il delay da usare nel timer di passaggio da un frame al segunte
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

void Widget::on_drawImageBtn_clicked()
{
    /*QString fileName = QFileDialog::getOpenFileName(this,tr("Load Image"),
                       "C:/", tr("Images (*.png *.bmp *.jpg)"));*/
    if(1/*fileName.length()>0*/){
        //imageDrawBuffer = new QImage("C:/Users/riccim3/Pictures/immagine.jpg" );
        imageDrawBuffer = new QImage("C:/immagine.jpg" );
        view->setTool(DRAWIMG);
    }

}

void Widget::on_imgHOffsetSlider_sliderMoved(int position)
{

}

void Widget::on_imgWSlider_sliderMoved(int position)
{

}

void Widget::on_imgHSlider_sliderMoved(int position)
{

}

void Widget::on_imgVOfsetSlider_sliderMoved(int position)
{

}

void Widget::on_imgRotationSlider_sliderMoved(int position)
{

}

void Widget::on_imgHOffsetSlider_sliderReleased()
{
    sliderFlags.sliderHOffset = false;
}

void Widget::on_imgVOfsetSlider_sliderReleased()
{
    sliderFlags.sliderVOffset = false;
}

void Widget::on_imgWSlider_sliderReleased()
{
    sliderFlags.sliderWScale = false;
}

void Widget::on_imgHSlider_sliderReleased()
{
    sliderFlags.sliderHScale = false;
}

void Widget::on_imgRotationSlider_sliderReleased()
{
    sliderFlags.sliderRot = false;
}



void Widget::on_saveLibraryBtn_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this,tr("save marionetta library"),
                       libFolder.path(),"Marionetta Library(*.marlib)");
    if(QFile::exists(fileName)){
        QDataStream myStream;

        view->saveLibrary(fileName);
        myLibraryListWidget->clear();
        detectLibraries();
    }
}



void Widget::on_addItemToLibBtn_clicked()
{
    view->addStickFigureToLibrary();
}

void Widget::on_loadLibraryBtn_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,tr("load marionetta library"),
                       libFolder.path(),"Marionetta Library(*.marlib)");
    if(QFile::exists(fileName)){
        QDataStream myStream;

        view->loadLibrary(fileName);
    }
}

void Widget::on_refreshLibrariesBtn_clicked()
{
    myLibraryListWidget->clear();
    detectLibraries();
}

void Widget::on_libraryList_currentRowChanged(int currentRow)
{
    QString fileName = "";
    int idx = myLibraryListWidget->currentRow();
    if(idx>=0 && idx<myLibraryListWidget->count()){
        QVariant buffer;
        buffer = myLibraryListWidget->item(idx)->data(Qt::UserRole);
        fileName = qvariant_cast<QString>(buffer);
        if(QFile::exists(fileName)){
            view->loadLibrary(fileName);
        }
    }

}

void Widget::on_removeFromLibraryBtn_clicked()
{
    view->removeFromLibrary();
}

void Widget::on_addItemFromLibraryToSceneBtn_clicked()
{
    StickFigure * added = addStick();
    // get selected item
    int idx = myCurrentLibraryWidget->currentRow();
    QListWidgetItem *itemToAdd = myCurrentLibraryWidget->item(idx);
    // estrai il dato utente salvato in esso, puntatore ad uno stickfigure
    QVariant  retrievedData = (itemToAdd->data(Qt::UserRole));
    //riconverti il dato
    StickFigure* toAdd = qvariant_cast<StickFigure*>(retrievedData);
    cloneStickFigure(added,toAdd);
    added->linkedItem->setData(Qt::DisplayRole,added->name);
    view->myAnimation->updateSelection(added);
    //added->refresh();
    //scene->update();
    //view->myAnimation->currentFrame->currentStickFigure->loadStickFigure(fileName)
}

void Widget::on_saveCurrentLibraryBtn_clicked()
{
    QString fileName = "";
    int idx = myLibraryListWidget->currentRow();
    if(idx>=0 && idx<myLibraryListWidget->count()){
        QVariant buffer;
        QListWidgetItem *item;
        item = myLibraryListWidget->item(idx);
        buffer = item->data(Qt::UserRole);
        fileName = qvariant_cast<QString>(buffer);
        if(QFile::exists(fileName)){
            QDataStream myStream;
            view->saveLibrary(fileName);
            myLibraryListWidget->clear();
            detectLibraries();
        }
    }
}

void Widget::on_imgHOffsetSlider_valueChanged(int value)
{
    stick*cs =  view->myAnimation->currentFrame->currentStickFigure->currentStick;
    if(cs == nullptr)
        return;
    else{
        if(cs->type == stick::IMAGE){
            if(!sliderFlags.sliderHOffset){
                view->storeUndo(CMD_SIMPLE);
                sliderFlags.sliderHOffset = true;
            }
            float val = 0;
            val = (float)imgHOffsetSlider->value();
            imgHOffsetSpinbox->setValue(imgHOffsetSlider->value());
            cs->imgOffset.setX(val);
             view->myAnimation->currentFrame->currentStickFigure->refresh();
        }
    }
}


void Widget::on_imgVOfsetSlider_valueChanged(int value)
{
    stick*cs =  view->myAnimation->currentFrame->currentStickFigure->currentStick;
    if(cs == nullptr)
        return;
    else{
        if(cs->type == stick::IMAGE){
            float val = 0;
            if(!sliderFlags.sliderVOffset){
                view->storeUndo(CMD_SIMPLE);
                sliderFlags.sliderVOffset = true;
            }
            imgVOffsetSpinbox->setValue(imgVOffsetSlider->value());
            val = (float)imgVOffsetSlider->value();
            cs->imgOffset.setY(val);
             view->myAnimation->currentFrame->currentStickFigure->refresh();
        }
    }
}

void Widget::on_imgWSlider_valueChanged(int value)
{
    stick*cs =  view->myAnimation->currentFrame->currentStickFigure->currentStick;
    if(cs == nullptr)
        return;
    else{
        if(cs->type == stick::IMAGE){
            float scale = 0;
            if(!sliderFlags.sliderWScale){
                view->storeUndo(CMD_SIMPLE);
                sliderFlags.sliderWScale = true;
            }
            imgWidthSlider->value()>0?scale = (float)imgWidthSlider->value()/10+1:scale = 1/((float)(-imgWidthSlider->value())/10+1);
            imgWidthSpinbox->setValue(imgWidthSlider->value());
            cs->imgWScale = scale;
            view->myAnimation->currentFrame->currentStickFigure->refresh();
        }
    }
}

void Widget::on_imgHSlider_valueChanged(int value)
{
    stick*cs =  view->myAnimation->currentFrame->currentStickFigure->currentStick;
    if(cs == nullptr)
        return;
    else{
        if(cs->type == stick::IMAGE){
            float scale = 0;
            if(!sliderFlags.sliderHScale){
                view->storeUndo(CMD_SIMPLE);
                sliderFlags.sliderHScale = true;
            }
            imgHeightSlider->value()>0?scale = (float)imgHeightSlider->value()/10+1 : scale = 1/((float)(-imgHeightSlider->value())/10+1);
            imgHeightSpinbox->setValue(imgHeightSlider->value());
            cs->imgHScale = scale;
             view->myAnimation->currentFrame->currentStickFigure->refresh();
        }
    }
}

void Widget::on_imgRotationSlider_valueChanged(int value)
{
    stick*cs =  view->myAnimation->currentFrame->currentStickFigure->currentStick;
    if(cs == nullptr)
        return;
    else{
        if(cs->type == stick::IMAGE){
            if(!sliderFlags.sliderRot){
                view->storeUndo(CMD_SIMPLE);
                sliderFlags.sliderRot = true;
            }
            float val = 0;
            val =imgRotationSlider->value();
            imgRotationSpinbox->setValue(val);
            cs->imgAngleOffset = val;
             view->myAnimation->currentFrame->currentStickFigure->refresh();
        }
    }
}

void Widget::on_imgHOffsetSpinbox_valueChanged(int arg1)
{
    imgHOffsetSlider->setValue(imgHOffsetSpinbox->value());
}

void Widget::on_imgVOffsetSpinbox_valueChanged(int arg1)
{
    imgVOffsetSlider->setValue(imgVOffsetSpinbox->value());
}

void Widget::on_imgWidthSpinbox_valueChanged(int arg1)
{
    imgWidthSlider->setValue(imgWidthSpinbox->value());
}

void Widget::on_imgHeightSpinbox_valueChanged(int arg1)
{
    imgHeightSlider->setValue(imgHeightSpinbox->value());
}

void Widget::on_imgRotationSpinbox_valueChanged(int arg1)
{
    imgRotationSlider->setValue(imgRotationSpinbox->value());
}
void Widget::on_chooseColorBtn_clicked()
{
    QColor color = QColorDialog::getColor(Qt::yellow, this );
    view->myPen.setColor(color);
    // aggiorna il colore del segnacolore
    if(color.isValid()) {
       QString qss = QString("background-color: %1").arg(color.name());
       ui->currentColorBtn->setStyleSheet(qss);
    }
}
void Widget::on_setStickColorBtn_clicked()
{
    view->setGraphics(false, ATTRIBUTE_PENCOLOR);
}

void Widget::on_setAllSticksColorBtn_clicked()
{
    view->setGraphics(true,ATTRIBUTE_PENCOLOR);
}

void Widget::on_setPenThicknessBtn_clicked()
{
    view->setGraphics(false, ATTRIBUTE_PENWIDTH);
}

void Widget::on_setPenThicknessAllBtn_clicked()
{
    view->setGraphics(true, ATTRIBUTE_PENWIDTH);
}

void Widget::on_selectBrushColorBtn_clicked()
{
    QColor color = QColorDialog::getColor(Qt::yellow, this );
    view->myBrush.setColor(color);
    // aggiorna il colore del segnacolore
    if(color.isValid()) {
       QString qss = QString("background-color: %1").arg(color.name());
       ui->currentBrushColorBtn->setStyleSheet(qss);
    }
    view->setGraphics(true,ATTRIBUTE_PENCOLOR);
}

void Widget::on_setAllBrushColorBtn_clicked()
{
    view->setGraphics(true, ATTRIBUTE_BRUSHCOLOR);
}

void Widget::on_setBrushColorBtn_clicked()
{
    view->setGraphics(false, ATTRIBUTE_BRUSHCOLOR);
}

void Widget::on_manipulateBtn_clicked()
{
    view->setTool(MANIPULATE);
}



void Widget::on_penOpacitySlider_valueChanged(int value)
{
    QColor c = view->myPen.color();
    c.setAlphaF((float)penOpacitySlider->value()/100);
    view->myPen.setColor(c);
    penOpacitySpinbox->setValue(penOpacitySlider->value());
}

void Widget::on_fillOpacitySlider_valueChanged(int value)
{
    QColor c = view->myBrush.color();
    c.setAlphaF((float)brushOpacitySlider->value()/100);
    view->myBrush.setColor(c);
    brushOpacitySpinbox->setValue(brushOpacitySlider->value());
}

void Widget::on_penOpacitySpinbox_valueChanged(int arg1)
{
    penOpacitySlider->setValue(penOpacitySpinbox->value());
}

void Widget::on_fillOpacitySpinbox_valueChanged(int arg1)
{
    brushOpacitySlider->setValue(brushOpacitySpinbox->value());
}


void Widget::on_rotateStickFigureBtn_clicked()
{
    view->setTool(ROTATE);
}
