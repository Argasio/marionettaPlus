#include "mymainwindow.h"
#include "ui_mymainwindow.h"
#include "animation.h"
#include "QFileDialog"
#include  <QDir>
#include <QProcess>
#include <QFileInfo>
#include <animationoptions.h>
#include <ffmpegexport.h>
#include <QMessageBox>
#define CS view->myAnimation->currentFrame->currentStickFigure->currentStick
#define CURRENTSTICKFIGURE view->myAnimation->currentFrame->currentStickFigure
#define CURRENTFRAME view->myAnimation->currentFrame
extern QDir programFolder;
extern int W;
extern int H;
extern int FPS;
extern int autoSaveInterval;
extern QGraphicsRectItem *myRect;
extern Widget *myWidget;
extern QString FFMPEGPath;
int opCount = 0;
bool renderFlag = false;
QDir renderFolder;
QString animationPath = "";
animationOptions* myOptions;
bool loadFile = false;

myMainWindow::myMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::myMainWindow)
{
    ui->setupUi(this);
}

myMainWindow::~myMainWindow()
{
    delete ui;
}

void myMainWindow::on_actionsave_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this,tr("Save StickFigure"),
                       "C:/", "Stickfigure (*.stck)");
    if(fileName.length()>1){
        QDataStream myStream;
        CURRENTSTICKFIGURE->saveStickFigure(fileName);
    }
}

void myMainWindow::on_actionload_triggered()
{

    QString fileName = QFileDialog::getOpenFileName(this,tr("Load StickFigure"),
                       "C:/", "Stickfigure (*.stck)");
    if(QFile::exists(fileName)){
        QDataStream myStream;
        centralW->addStick();
        CURRENTSTICKFIGURE->loadStickFigure(fileName);
        for(stick*s:CURRENTSTICKFIGURE->stickList){
            CURRENTFRAME->totalSticks.append(s);
        }
    }
}

void myMainWindow::on_actionsave_animation_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this,tr("Save Animation"),
                       "C:/", "Marionetta Animation (*.mAnim)");
    animationPath = fileName;
    if(fileName.length()>1){
        view->myAnimation->saveAnimation(fileName);
        opCount = 0;
    }
}

void myMainWindow::on_actionload_animation_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,tr("Load Animation"),
                       "C:/", "Marionetta Animation (*.mAnim)");
     view->clearUndo();
     animationPath = fileName;
    if(QFile::exists(fileName)){
        loadFile = true;
        QDataStream myStream;
        view->myAnimation->loadAnimation(fileName);
        view->moveToFrame(view->myAnimation->frameList[0]);
        loadFile = false;
        opCount = 0;
    }
}

void myMainWindow::on_actionexport_as_AVI_file_triggered()
{
    //QVideoEncoder encoder;
    /*

*/
    // generate folder for renderings
    QString fileName = QFileDialog::getSaveFileName(this,tr("Export animation as an avi file"),
                       "C:/", "avi video file (*.avi)");
    QString path = programFolder.path()+"/render";
    renderFolder = QDir(path);
    if(renderFolder.exists()){
        qDebug("folder located");
    }
    else{
        qDebug("folder created");
        renderFolder.mkpath(path);
    }
    renderFolder.setPath(path);
    // per ogni frame crea una scena temporanea, popolala con cloni degli sticks del progetto,
    // renderizza la scena su una QImage tramite un painter e salva l'immagine nella cartella
    emptyDirectory( path);
    exportImageSequence(path);
    // apri il programma avi2Image, passagli i parametri per iniziale la conversione da immagini a file video
    // 1 in folder, 2 outfolder, 3 fps, 4 larghezza, 5 altezza, n immagini
    QProcess *p = new QProcess(this);
    QStringList paramList;
    paramList<<renderFolder.path()
                    <<fileName
                    <<"24"
                    <<QString::number(W)
                    <<QString::number(H)
                    <<QString::number(view->myAnimation->frameList.count());
    p->start("C:/Users/Argasio/Documents/GitHub/MarionettaPlus/marionettaPlus/image2Avi.exe");
    if (!p->waitForStarted())
        qDebug("starting render problem");
    for(QString s: paramList){
        p->write(s.toUtf8(),s.length());
        p->write("\n");
    }
}
void myMainWindow::exportImageSequence(QString path){
    renderFlag = true;
    for(Frame* f :view->myAnimation->frameList){
        QGraphicsScene renderScene; //scena fittizia
        QImage *renderImg = new QImage(W,H,QImage::Format_ARGB32); //immagine temporanea
        QPainter painter(renderImg); // painter che esegue il drawing sull immagine
        renderScene.setSceneRect(myRect->rect());
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
        //disegna sull' immagine e salvala
        renderImg->fill(Qt::white);
        painter.setBackground(QBrush(QColor(Qt::white)));
        renderScene.render(&painter);
        QString itemPath = path;
        itemPath.insert(path.lastIndexOf("."),"_"+QString::number(f->frameNumber));
        renderImg->save(itemPath);
        painter.end();
        delete renderImg;
        renderScene.clear();
        //...add all other video frames here
    }
    renderFlag = false;
}
void myMainWindow::on_actionsave_animation_current_triggered()
{
    if(QFile::exists(animationPath)){
        view->myAnimation->saveAnimation(animationPath);
        opCount = 0;
    }
    else{
        on_actionsave_animation_triggered();
    }
}

void myMainWindow::on_actionNew_Blank_Animation_triggered()
{
    view->myAnimation->clearAnimation();
    myWidget->addFrame();
    opCount = 0;
}
void myMainWindow::getAnimationParams(int optionVal){
    FPS = myOptions->fps;
    W   = myOptions->width;
    H   = myOptions->height;
    view->sizeChange(optionVal);
    myWidget->writeJson();
    autoSaveInterval = myOptions->autoSaveInterval;
    delete myOptions;
}
void myMainWindow::on_actionAnimation_Options_triggered()
{
    myOptions = new animationOptions(nullptr,W,H,FPS,autoSaveInterval);

    myOptions->show();
    QObject::connect(myOptions, &animationOptions::dialogOut,this, &myMainWindow::getAnimationParams);
}

void myMainWindow::on_actionExport_video_using_FFMPEG_triggered()
{
    if(QFile::exists(FFMPEGPath)){
        exportUsingFFMPEG();
    }
    else{
        QMessageBox::about(this,"FFMPEG Location","Please locate where ffmpeg executable is located on your system");
        QString fileName = QFileDialog::getOpenFileName(this,tr("find FFMPEG executable file"),
                           "C:/", "ffmpeg(*.exe)");
        if(QFile::exists(fileName)){
                if(QFileInfo(QFile(fileName)).fileName()=="ffmpeg.exe"){
                    FFMPEGPath = fileName;
                    myWidget->writeJson();
                    exportUsingFFMPEG();
                }
        }
    }
}

ffmpegExport* ffmpegDialog;
void myMainWindow::exportUsingFFMPEG(){
    ffmpegDialog = new ffmpegExport;
    ffmpegDialog->show();
    QObject::connect(ffmpegDialog, &ffmpegExport::exportVideo,this, &myMainWindow::startffmpegRender);
}

void myMainWindow::startffmpegRender(QStringList options){
    // per ogni frame crea una scena temporanea, popolala con cloni degli sticks del progetto,
    // renderizza la scena su una QImage tramite un painter e salva l'immagine nella cartella
    emptyDirectory( renderFolder.path());
    exportImageSequence(renderFolder.path()+"/img.png");
    //

    //QString imagePath = programFolder.path()+"/render";

    QProcess *p = new QProcess(this);

    p->start(FFMPEGPath,options);
}

void myMainWindow::autoSave()
{
    on_actionsave_animation_current_triggered();
}

void myMainWindow::on_actionExport_as_image_sequence_triggered()
{

    QString fileName = QFileDialog::getSaveFileName(this,tr("Save animation as a sequence of images"),
                       "C:/", "PNG Image (*.png)");
    if(fileName.length()>0)
        exportImageSequence(fileName);
}
void myMainWindow::doConnections(){
        QObject::connect(view, &myView::autoSaveTrigger,this, &myMainWindow::autoSave);
}
void myMainWindow::closeEvent(QCloseEvent *event)
{
    if(opCount>0){
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Save", "There is unsaved progress in your work, do you want to save your changes?",
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        if (reply == QMessageBox::Save) {
            on_actionsave_animation_current_triggered();
            QMainWindow::closeEvent(event);

        }
        else if (reply == QMessageBox::Discard)
        {

        }
        else if(reply == QMessageBox::Cancel)
        {
            event->ignore();
            return;
        }

    }
    else
        QMainWindow::closeEvent(event);
}
