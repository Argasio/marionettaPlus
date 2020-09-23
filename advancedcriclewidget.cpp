#include "advancedcriclewidget.h"
#include "ui_advancedcriclewidget.h"

advancedCricleWidget::advancedCricleWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::advancedCricleWidget)
{
    ui->setupUi(this);
}

advancedCricleWidget::~advancedCricleWidget()
{
    delete ui;
}
