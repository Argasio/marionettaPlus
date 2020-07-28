#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QListWidget>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_drawBtn_released();

    void on_cursor_clicked();

    void on_drawBtn_clicked();

    void on_drawCircleBtn_clicked();

    void on_addStickBtn_clicked();

    void on_deleteStickFigureBtn_clicked();

    void on_moveLayerUpBtn_clicked();

    void on_moveLayerDownBtn_clicked();

    void on_thicknessSpinBox_valueChanged(int arg1);

    void on_stickLayerView_itemClicked(QListWidgetItem *item);

    void on_stickLayerView_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

    void on_actionsave_stickfigure_triggered();

    void on_actionopen_stickfigure_triggered();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
