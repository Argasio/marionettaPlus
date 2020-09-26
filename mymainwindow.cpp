#include "mymainwindow.h"
#include "ui_mymainwindow.h"
#include "animation.h"
#include "QFileDialog"
#include  <QDir>
#include <QProcess>
#define CS view->myAnimation->currentFrame->currentStickFigure->currentStick
#define CURRENTSTICKFIGURE view->myAnimation->currentFrame->currentStickFigure
#define CURRENTFRAME view->myAnimation->currentFrame
extern QDir programFolder;
extern int W;
extern int H;
extern QGraphicsRectItem *myRect;
QString animationPath = "";

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
    }
}

void myMainWindow::on_actionsave_animation_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this,tr("Save Animation"),
                       "C:/", "Marionetta Animation (*.mAnim)");
    animationPath = fileName;
    if(fileName.length()>1)
        view->myAnimation->saveAnimation(fileName);
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
    }
}

void myMainWindow::on_actionexport_as_AVI_file_triggered()
{
    //QVideoEncoder encoder;
    /*
    QProcess *p = new QProcess(this);
    QStringList paramList;
    paramList<<"-i"<<"C:/Users/Argasio/Documents/GitHub/MarionettaPlus/MarionettaPlus/input.mp4"
            <<"C:/Users/Argasio/Documents/GitHub/MarionettaPlus/MarionettaPlus/output.webm";
    p->startDetached("C:/ffmpeg-20200814-a762fd2-win64-static/bin/ffmpeg.exe",paramList);*/
    // generate folder for renderings
    QString path = programFolder.path()+"/render";
    QDir renderFolder = QDir(path);
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
        renderImg->fill(Qt::transparent);
        painter.setBackground(QBrush(QColor(Qt::transparent)));
        renderScene.render(&painter);
        renderImg->save(renderFolder.path()+"/render_"+QString::number(f->frameNumber)+".png");
        painter.end();
        delete renderImg;
        renderScene.clear();
        //...add all other video frames here
    }
    // apri il programma avi2Image, passagli i parametri per iniziale la conversione da immagini a file video
    QProcess *p = new QProcess(this);
    QStringList paramList;
    paramList<<renderFolder.path()
                    <<renderFolder.path()
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

void myMainWindow::on_actionsave_animation_current_triggered()
{
    if(QFile::exists(animationPath)){
        view->myAnimation->saveAnimation(animationPath);
    }
    else{
        on_actionsave_animation_triggered();
    }
}
