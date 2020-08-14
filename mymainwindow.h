#ifndef MYMAINWINDOW_H
#define MYMAINWINDOW_H

#include <QMainWindow>
#include "myview.h"
#include "widget.h"
namespace Ui {
class myMainWindow;
}

class myMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit myMainWindow(QWidget *parent = nullptr);
    ~myMainWindow();
    myView *view;
    Widget *centralW;

private slots:
    void on_actionsave_triggered();

    void on_actionload_triggered();

    void on_actionsave_animation_triggered();

    void on_actionload_animation_triggered();

private:
    Ui::myMainWindow *ui;
};

#endif // MYMAINWINDOW_H
