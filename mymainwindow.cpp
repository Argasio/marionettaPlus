#include "mymainwindow.h"
#include "ui_mymainwindow.h"
#include "animation.h"
#include "QFileDialog"
#include "imageexport.h"
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
extern QDir tempRenderFolder;
extern QDir myAnimationsFolder;
extern QDir myStickFiguresFolder;
extern QDir myVideoExportFolder;
int opCount = 0;
bool renderFlag = false;
QDir renderFolder;
QString animationPath = "";
animationOptions* myOptions;
bool loadFile = false;
imageExport *exporter;
QString aviExportFolder;
QStringList ffmpegOptions;
myMainWindow::myMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::myMainWindow)
{
    ui->setupUi(this);
    checkUpdate* Check = new checkUpdate();
    QObject::connect(Check, &checkUpdate::updatePresent,this, &myMainWindow::updatePresent);
    Check->start();
}


myMainWindow::~myMainWindow()
{
    delete ui;
}

void myMainWindow::on_actionsave_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this,tr("Save StickFigure"),
                       myStickFiguresFolder.path(), "Stickfigure (*.stck)");
    if(fileName.length()>1){
        QDataStream myStream;
        CURRENTSTICKFIGURE->saveStickFigure(fileName);
    }
}

void myMainWindow::on_actionload_triggered()
{

    QString fileName = QFileDialog::getOpenFileName(this,tr("Load StickFigure"),
                       myStickFiguresFolder.path(), "Stickfigure (*.stck)");
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
                       myAnimationsFolder.path(), "Marionetta Animation (*.mAnim)");
    animationPath = fileName;
    if(fileName.length()>1){
        view->myAnimation->saveAnimation(fileName);
        opCount = 0;
    }
}

void myMainWindow::on_actionload_animation_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,tr("Load Animation"),
                       myAnimationsFolder.path(), "Marionetta Animation (*.mAnim)");
     view->clearUndo();
     animationPath = fileName;
    if(QFile::exists(fileName)){
        loadFile = true;
        QDataStream myStream;
        view->myAnimation->loadAnimation(fileName);
        if(view->myAnimation->width != W || view->myAnimation->height != H){
            W = view->myAnimation->width;
            H = view->myAnimation->height;
            view->sizeChange(NOREPO);
        }
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
    aviExportFolder = QFileDialog::getSaveFileName(this,tr("Export animation as an avi file"),
                       myVideoExportFolder.path(), "avi video file (*.avi)");
    QString path = programFolder.path()+"/render";
    renderFolder = QDir(path);
    if(aviExportFolder.length() == 0)
        return;
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
    QString fullpath  = path+"/render.png";
    exportImageSequence(fullpath, AVI);

}
void myMainWindow::exportAvi(){
    // generate folder for renderings
    QString fileName = aviExportFolder;
    QString imgPath = programFolder.path()+"/render";
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
    // la path in cui si trova l'é'eseguibile di marionetta
    QString exePath = QCoreApplication::applicationDirPath();
    // estrai il file exporter di avi
    QFile::copy(":/image2Avi.exe",exePath+"/image2Avi.exe");
    // eseguilo
    p->start(exePath+"/image2Avi.exe");
    if (!p->waitForStarted())
        qDebug("starting render problem");
    for(QString s: paramList){
        p->write(s.toUtf8(),s.length());
        p->write("\n");
    }
    p->waitForFinished();
    QString output = (p->readAllStandardOutput());
    delete p;

}
void myMainWindow::exportImageSequence(QString path, int option){

    exporter = new imageExport(this,path,option);
    QObject::connect(exporter,&imageExport::imageExportFinished,this,&myMainWindow::imageRenderOver);
    exporter->start();

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

    int ret  = QMessageBox::warning(this, tr("Warning"),
                                    tr("This will delete all frames int eh current workspace, this action cannot be undone.\n"
                                       "Do you want to proceed?"),
                                    QMessageBox::Yes | QMessageBox::No
                                    );
    if(ret == QMessageBox::Yes)
    {
        view->myAnimation->clearAnimation();
        myWidget->addFrame();
        opCount = 0;
    }
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
                           myVideoExportFolder.path(), "ffmpeg(*.exe)");
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
    ffmpegOptions = options;
    emptyDirectory( renderFolder.path());
    exportImageSequence(renderFolder.path()+"/img.png", FFMPEG);
    //

    //QString imagePath = programFolder.path()+"/render";


}

void myMainWindow::autoSave()
{
    on_actionsave_animation_current_triggered();
}

void myMainWindow::updatePresent(QString ver)
{
    QMessageBox::about(this,"Old version Alert", "A newer version of this Software is available at Marionetta's website for download!");
}
// se questa funzione è stata chiamata per l'auto render avi, chiama una funzione che apra il processo
// stessa cosa se originariamente l'esportazione delle immagini dovrà invece essere usata da ffmpeg
void myMainWindow::imageRenderOver(int option){

    switch(option){
    case NONE:

        break;
    case AVI:
        exportAvi();
        break;
    case FFMPEG:
        QProcess *p = new QProcess(this);

        p->start(FFMPEGPath,ffmpegOptions);
        p->deleteLater();
        break;
    }
}


void myMainWindow::on_actionExport_as_image_sequence_triggered()
{

    QString fileName = QFileDialog::getSaveFileName(this,tr("Save animation as a sequence of images"),
                       "C:/", "PNG Image (*.png)");
    if(fileName.length()>0)
        exportImageSequence(fileName,NONE);
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

void myMainWindow::on_actionAbout_triggered()
{
    delete exporter;
}
