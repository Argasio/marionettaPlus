#include "advancedcirclewidget.h"
#include "ui_advancedcirclewidget.h"
#include "uiItems.h"
extern myView *V;
#define CS V->myAnimation->currentFrame->currentStickFigure->currentStick
#define CURRENTSTICKFIGURE V->myAnimation->currentFrame->currentStickFigure
#define CURRENTFRAME V->myAnimation->currentFrame

advancedCircleWidget::advancedCircleWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::advancedCircleWidget)
{
    ui->setupUi(this);
    circleSquashSlider = ui->circleSquashSlider;
    circleSquashSpinbox = ui->circleSquashSpinbox;
}

advancedCircleWidget::~advancedCircleWidget()
{
    delete ui;
}

void advancedCircleWidget::on_circleSquashSlider_valueChanged(int value)
{
    circleSquashSpinbox->setValue(value);
    if(CURRENTSTICKFIGURE!=nullptr){
        if(CS!=nullptr){
            if(CS->stickType== stick::CIRCLE){
                if(value<0){

                    CS->imgWScale = (1-(-(float)value)/100);
                }
                else{
                    CS->imgWScale = (float)(1+(float)value/20);
                }
                CS->refresh(0);
            }
        }
    }
}
