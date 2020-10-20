#include "advancedlinewidget.h"
#include "ui_advancedlinewidget.h"
#include "myview.h"
#include "animation.h"
#include "stick.h"
#include "uiItems.h"
extern int W;
extern int H;

#define CS V->myAnimation->currentFrame->currentStickFigure->currentStick
#define CURRENTSTICKFIGURE V->myAnimation->currentFrame->currentStickFigure
#define CURRENTFRAME V->myAnimation->currentFrame
extern myView *V;
advancedLineWidget::advancedLineWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::advancedLineWidget)
{
    ui->setupUi(this);
    editLineThicknessSlider = ui->editLineThicknessSlider;
    lineEditThicknessWidthSpinbox = ui->lineEditThicknessWidthSpinbox;
}

advancedLineWidget::~advancedLineWidget()
{
    delete ui;
}

void advancedLineWidget::on_editLineThicknessSlider_valueChanged(int value)
{
    if(CURRENTSTICKFIGURE != nullptr){
        if(CS != nullptr){
            editLineThicknessSlider->setRange(0,0.5*sqrt(pow(W,2)+pow(H,2)));
            lineEditThicknessWidthSpinbox->setRange(0,0.5*sqrt(pow(W,2)+pow(H,2)));
            lineEditThicknessWidthSpinbox->setValue(value);
            CS->Pen.setWidth(value);
            V->myPen.setWidth(value);
            CS->refresh(1);
        }
    }
}

void advancedLineWidget::on_lineEditThicknessWidthSpinbox_editingFinished()
{
    editLineThicknessSlider->setValue(lineEditThicknessWidthSpinbox->value());
}
