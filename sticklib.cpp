#include "sticklib.h"
#include "ui_sticklib.h"

stickLib::stickLib(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::stickLib)
{
    ui->setupUi(this);
}

stickLib::~stickLib()
{
    delete ui;
}
