#include "advancedtab.h"
#include "ui_advancedtab.h"
#include "myview.h"
extern int W;
extern int H;
extern myView *V;
bool rectWSliderFlag = false;
advancedTab::advancedTab(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::advancedTab)
{
    ui->setupUi(this);
    ui->rectWidthSlider->setRange(0,W);
    ui->rectWidthSpinbox->setRange(0,W);
}

advancedTab::~advancedTab()
{
    delete ui;
}

void advancedTab::on_imgHOffsetSpinbox_valueChanged(int arg1)
{

}

void advancedTab::on_rectWidthSlider_valueChanged(int value)
{
    ui->rectWidthSpinbox->setValue(value);
    if(V->myAnimation->currentFrame->currentStickFigure->currentStick==nullptr)
        return;
    if(rectWSliderFlag == false){
        rectWSliderFlag = true;

        V->storeUndo();
    }
    V->myAnimation->currentFrame->currentStickFigure->currentStick->imgWScale = value;
    V->myAnimation->currentFrame->currentStickFigure->refresh();

}

void advancedTab::on_rectWidthSpinbox_valueChanged(int arg1)
{
    ui->rectWidthSlider->setValue(arg1);
    if(V->myAnimation->currentFrame->currentStickFigure->currentStick==nullptr)
        return;
    V->myAnimation->currentFrame->currentStickFigure->currentStick->imgWScale = arg1;
    V->myAnimation->currentFrame->currentStickFigure->refresh();
    ui->rectWidthSlider->setValue(arg1);
}

void advancedTab::on_rectWidthSlider_sliderReleased()
{
    rectWSliderFlag = false;
}
