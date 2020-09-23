#ifndef ADVANCEDTAB_H
#define ADVANCEDTAB_H
#include "myview.h"
#include <QWidget>

namespace Ui {
class advancedTab;
}

class advancedTab : public QWidget
{
    Q_OBJECT

public:
    explicit advancedTab(QWidget *parent = nullptr);
    ~advancedTab();

private slots:
    void on_imgHOffsetSpinbox_valueChanged(int arg1);

    void on_rectWidthSlider_valueChanged(int value);

    void on_rectWidthSpinbox_valueChanged(int arg1);

    void on_rectWidthSlider_sliderReleased();

private:
    Ui::advancedTab *ui;

};

#endif // ADVANCEDTAB_H
