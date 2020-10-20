#ifndef ADVANCEDCIRCLEWIDGET_H
#define ADVANCEDCIRCLEWIDGET_H

#include <QWidget>

namespace Ui {
class advancedCircleWidget;
}

class advancedCircleWidget : public QWidget
{
    Q_OBJECT

public:
    explicit advancedCircleWidget(QWidget *parent = nullptr);
    ~advancedCircleWidget();

private slots:
    void on_circleSquashSlider_valueChanged(int value);

private:
    Ui::advancedCircleWidget *ui;
};

#endif // ADVANCEDCIRCLEWIDGET_H
