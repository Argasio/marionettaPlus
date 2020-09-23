#ifndef ADVANCEDLINEWIDGET_H
#define ADVANCEDLINEWIDGET_H

#include <QWidget>

namespace Ui {
class advancedLineWidget;
}

class advancedLineWidget : public QWidget
{
    Q_OBJECT

public:
    explicit advancedLineWidget(QWidget *parent = nullptr);
    ~advancedLineWidget();

private:
    Ui::advancedLineWidget *ui;
};

#endif // ADVANCEDLINEWIDGET_H
