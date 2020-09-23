#include "advancedcirclewidget.h"
#include "ui_advancedcirclewidget.h"

advancedCircleWidget::advancedCircleWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::advancedCircleWidget)
{
    ui->setupUi(this);
}

advancedCircleWidget::~advancedCircleWidget()
{
    delete ui;
}
