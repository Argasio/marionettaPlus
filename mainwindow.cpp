#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <widget.h>
#include "widget.h"
#include "ui_widget.h"
#include <QGraphicsRectItem>
#include <QGraphicsView>
#include <QGraphicsScene>
#include "myview.h"
#include <frame.h>
#include <animation.h>
#include <QPen>
#include <QFileDialog>
#include <QIcon>
#include <QVariant>
#include <QDebug>
#include <QMenuBar>
QGraphicsScene *scene;
myView *view;
QList <StickFigure*> layerList;
QListWidget * myStickFigureWidgetList;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //crea la scena virtuale
    scene = new QGraphicsScene(this);
    //crea il pannello e collegalo alla scena
    view = new myView(this);
    view->setScene(scene);
    view->myAnimation->scene = scene;
    //inserisci il pannello nella finestra
    ui->viewLayout->addWidget(view);
    //crea la cornice
    QGraphicsRectItem *item = new QGraphicsRectItem(0, 0, 600, 600);
    scene->addItem(item);
    myStickFigureWidgetList = ui->stickLayerView;
    on_addStickBtn_clicked();

}

MainWindow::~MainWindow()
{
    delete ui;
}




void MainWindow::on_drawBtn_released()
{
    static int i = 0;
    i++;
}

void MainWindow::on_drawBtn_clicked()
{
    view->setTool(DRAW);
}

void MainWindow::on_cursor_clicked()
{
    view->setTool(NOTOOL);
}

void MainWindow::on_addStickBtn_clicked()
{
    static unsigned int intName = 0;
    intName++;
    QString name;
    name.sprintf("%d",intName);
    QListWidgetItem * addedItem = new QListWidgetItem(ui->stickLayerView);
    StickFigure* addedStickFigure = view->addStickFigure( addedItem);
    QVariant newData(QVariant::fromValue(addedStickFigure));
    addedItem->setData(Qt::UserRole,newData);
    addedItem->setData(Qt::DisplayRole,name);
    addedItem->setIcon(*addedStickFigure->stickFigureIcon);
    view->myAnimation->updateSelection(addedStickFigure);
    ui->stickLayerView->clearSelection();
    ui->stickLayerView->setItemSelected(addedItem, true);
}

void MainWindow::on_thicknessSpinBox_valueChanged(int arg1)
{
    myPen.setWidth(arg1);
}

void MainWindow::on_stickLayerView_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    if(current != nullptr)
    {
        qDebug("layer clicked");
        QVariant  retrievedData = (current->data(Qt::UserRole));
        StickFigure* cs = qvariant_cast<StickFigure*>(retrievedData);
        view->myAnimation->updateSelection(cs);
    }
}

void MainWindow::on_stickLayerView_itemClicked(QListWidgetItem *item)
{

}

void MainWindow::on_deleteStickFigureBtn_clicked()
{
    view->deleteStickFigure();
}

void MainWindow::on_moveLayerUpBtn_clicked()
{
    view->moveStickFigureZ(1);
}

void MainWindow::on_moveLayerDownBtn_clicked()
{
    view->moveStickFigureZ(-1);
}

void MainWindow::on_drawCircleBtn_clicked()
{
    view->setTool(DRAWCIRCLE);
}

void MainWindow::on_actionsave_stickfigure_triggered()
{
    QString fileName = "";
    fileName = QFileDialog::getSaveFileName(this,"save StickFigure","C://",tr("Marionetta (*.mar)"));
    view->saveStickFigure(fileName);
}

void MainWindow::on_actionopen_stickfigure_triggered()
{
    QString fileName = "";
    fileName = QFileDialog::getOpenFileName(this,"load StickFigure","C://",tr("Marionetta (*.mar)"));
    on_addStickBtn_clicked();
    view->loadStickFigure(fileName);


}
