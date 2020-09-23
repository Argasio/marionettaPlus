#ifndef ADVANCEDTAPERWIDGET_H
#define ADVANCEDTAPERWIDGET_H

#include <QWidget>

namespace Ui {
class advancedTaperWidget;
}

class advancedTaperWidget : public QWidget
{
    Q_OBJECT

public:
    explicit advancedTaperWidget(QWidget *parent = nullptr);
    ~advancedTaperWidget();

private slots:
    void on_taperWidthSlider_valueChanged(int value);

    void on_taperWidthSlider_sliderReleased();

    void on_taperHeightSpinbox_valueChanged(int arg1);

    void on_taperHeightSlider_valueChanged(int value);

    void on_taperHeightSlider_sliderReleased();

    void on_taperWidthSpinbox_valueChanged(int arg1);

private:
    Ui::advancedTaperWidget *ui;
};

#endif // ADVANCEDTAPERWIDGET_H
