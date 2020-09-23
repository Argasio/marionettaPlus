#include "advancedlinewidget.h"
#include "ui_advancedlinewidget.h"

advancedLineWidget::advancedLineWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::advancedLineWidget)
{
    ui->setupUi(this);
}

advancedLineWidget::~advancedLineWidget()
{
    delete ui;
}
