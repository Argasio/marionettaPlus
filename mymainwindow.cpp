#include "mymainwindow.h"
#include "ui_mymainwindow.h"
#include "animation.h"
#include "QFileDialog"
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
        centralW->on_addStickBtn_clicked();
        view->myAnimation->currentFrame->currentStickFigure->loadStickFigure(filename);
    }
}
