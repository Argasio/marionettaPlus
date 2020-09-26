#include "advancedtab.h"
#include "ui_advancedtab.h"
#include "myview.h"
extern int W;
extern int H;
extern myView *V;
bool rectWSliderFlag = false;
QSlider* advancedRectSlider;

#define CS V->myAnimation->currentFrame->currentStickFigure->currentStick
#define CURRENTSTICKFIGURE V->myAnimation->currentFrame->currentStickFigure
#define CURRENTFRAME V->myAnimation->currentFrame
advancedTab::advancedTab(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::advancedTab)
{
    ui->setupUi(this);
    ui->rectWidthSlider->setRange(0,W);
    ui->rectWidthSpinbox->setRange(0,W);
    advancedRectSlider = ui->rectWidthSlider;
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
    if(CS==nullptr)
        return;
    if(rectWSliderFlag == false){
        rectWSliderFlag = true;

        V->storeUndo();
    }
    CS->imgWScale = value;
    CURRENTSTICKFIGURE->refresh();

}

void advancedTab::on_rectWidthSpinbox_valueChanged(int arg1)
{
    ui->rectWidthSlider->setValue(arg1);
    if(CS==nullptr)
        return;
    CS->imgWScale = arg1;
    CURRENTSTICKFIGURE->refresh();
    ui->rectWidthSlider->setValue(arg1);
}

void advancedTab::on_rectWidthSlider_sliderReleased()
{
    rectWSliderFlag = false;
}
