#include "animationoptions.h"
#include "ui_animationoptions.h"

#include <QMessageBox>

float AR = 1.0;
animationOptions::animationOptions(QWidget *parent, int W, int H, int FPS,int handles, int autoSaveInput) :
    QWidget(parent),
    ui(new Ui::animationOptions)
{
    ui->setupUi(this);
    ui->widthSpinbox->setValue(W);
    ui->heightSpinbox->setValue(H);
    ui->fpsSpinbox->setValue(FPS);
    ui->handlesSizeSpinBox->setValue(handles);
    ui->autoSaveSpinbox->setValue(autoSaveInterval);
    AR = (float)W/(float)H;
}

animationOptions::~animationOptions()
{
    delete ui;
}

void animationOptions::on_okBtn_clicked()
{
    int optionVal;
    width               = ui->widthSpinbox->value();
    height              = ui->heightSpinbox->value();
    fps                 = ui->fpsSpinbox->value();
    autoSaveInterval    = ui->autoSaveSpinbox->value();
    handleSize          = ui->handlesSizeSpinBox->value();

    if(ui->repositionBtn->isChecked())
        optionVal = REPO;
    else if(ui->rescaleAndRepositionBtn->isChecked())
        optionVal = REPOSCALE;
    else if(ui->noneBtn->isChecked())
        optionVal = NOREPO;
    int ret  = QMessageBox::warning(this, tr("My Application"),
                                    tr("This will rescale the animation, this action cannot be undone.\n"
                                       "Do you want to proceed?"),
                                    QMessageBox::Yes | QMessageBox::No
                                    );
    if(ret == QMessageBox::Yes)
        emit dialogOut(optionVal);

}

void animationOptions::on_widthSpinbox_valueChanged(int arg1)
{

}

void animationOptions::on_heightSpinbox_valueChanged(int arg1)
{

}

void animationOptions::on_widthSpinbox_editingFinished()
{
    if(ui->lockAspectRatioCheckbox->isChecked()){
        ui->heightSpinbox->setValue((int)qRound(ui->widthSpinbox->value()/AR));
    }
}

void animationOptions::on_heightSpinbox_editingFinished()
{
    if(ui->lockAspectRatioCheckbox->isChecked()){
        ui->widthSpinbox->setValue((int)qRound(ui->heightSpinbox->value()*AR));
    }
}

void animationOptions::on_handlesSizeSpinBox_valueChanged(int arg1)
{

}
