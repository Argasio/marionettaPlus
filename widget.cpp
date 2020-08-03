#include "widget.h"
#include "ui_widget.h"
#include <QGraphicsRectItem>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <frame.h>
#include <animation.h>
#include <QPen>
#include <QIcon>
#include <QVariant>
#include <QDebug>
QGraphicsScene *scene;

QList <StickFigure*> layerList;
QListWidget * myStickFigureWidgetList;
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
    view->myAnimation->scene = scene;
    //inserisci il pannello nella finestra
    ui->viewLayout->addWidget(view);
    //crea la cornice
    QGraphicsRectItem *item = new QGraphicsRectItem(0, 0, 600, 600);
    scene->addItem(item);
    myStickFigureWidgetList = ui->stickLayerView;
    on_addStickBtn_clicked();


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

void Widget::on_thicknessSpinBox_valueChanged(int arg1)
{
    myPen.setWidth(arg1);
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
