#include "widget.h"
#include "ui_widget.h"
#include <QGraphicsRectItem>
#include <QGraphicsView>
#include <QGraphicsScene>
#include "myview.h"
#include <frame.h>
#include <animation.h>
#include <QPen>
#include <QIcon>
QGraphicsScene *scene;
myView *view;
QListWidget *stickListView;
extern Animation myAnimation;
QList <StickFigure*> layerList;
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    //crea la scena virtuale
    scene = new QGraphicsScene(this);
    //crea il pannello e collegalo alla scena
    view = new myView(this);
    view->setScene(scene);
    //inserisci il pannello nella finestra
    ui->viewLayout->addWidget(view);
    //crea la cornice
    QGraphicsRectItem *item = new QGraphicsRectItem(0, 0, 600, 600);
    scene->addItem(item);
    on_addStickBtn_clicked();
    stickListView = ui->stickLayerView;
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
    QString pointer;
    QString name;
    name.sprintf("%d",ui->stickLayerView->count());

    //intptr_t myPointer = (intptr_t)stickFigurePtr;
    //pointer.sprintf("%p",(void*)stickFigurePtr);
    QIcon myIco = QIcon(QString("C:/Users/Argasio/Pictures/atteya.jpg"));
    ui->stickLayerView->addItem(myIco,name);

    int idx = ui->stickLayerView->count()-1;
    layerList.insert(idx,view->addStickFigure());

}

void Widget::on_thicknessSpinBox_valueChanged(int arg1)
{
    myPen.setWidth(arg1);
}


void Widget::on_stickLayerView_itemClicked(QListWidgetItem *item)
{
    int idx = (item->data(Qt::DisplayRole).toString()).toInt();
    //layerList[idx]->currentStick->Pen.setColor(Qt::green);
    //layerList[idx]->currentStick->refresh(0);
    myAnimation.updateSelection(layerList[idx]);

}
