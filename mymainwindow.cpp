#include "mymainwindow.h"
#include "ui_mymainwindow.h"
#include "animation.h"
#include "QFileDialog"
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
    QString filename = QFileDialog::getSaveFileName(this,tr("Save StickFigure"),
                       "C:/", "Stickfigure (*.stck)");
    QDataStream myStream;
    view->myAnimation->currentFrame->currentStickFigure->saveStickFigure(filename);
}

void myMainWindow::on_actionload_triggered()
{

    QString filename = QFileDialog::getOpenFileName(this,tr("Load StickFigure"),
                       "C:/", "Stickfigure (*.stck)");
    if(filename.length()>0){
        QDataStream myStream;
        centralW->addStick();
        view->myAnimation->currentFrame->currentStickFigure->loadStickFigure(filename);
    }
}

void myMainWindow::on_actionsave_animation_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this,tr("Save Animation"),
                       "C:/", "Marionetta Animation (*.mAnim)");
    view->myAnimation->saveAnimation(fileName);
}

void myMainWindow::on_actionload_animation_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this,tr("Load Animation"),
                       "C:/", "Marionetta Animation (*.mAnim)");
     view->clearUndo();
    if(filename.length()>0){
        loadFile = true;
        QDataStream myStream;
        view->myAnimation->loadAnimation(filename);
        view->moveToFrame(view->myAnimation->frameList[0]);
        loadFile = false;
    }
}
