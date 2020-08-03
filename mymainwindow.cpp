#include "mymainwindow.h"
#include "ui_mymainwindow.h"

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
