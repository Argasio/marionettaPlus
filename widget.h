#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QListWidget>
QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void on_drawBtn_clicked();

    void on_cursor_clicked();

    void on_addStickBtn_clicked();

    void on_thicknessSpinBox_valueChanged(int arg1);

    void on_stickLayerView_itemClicked(QListWidgetItem *item);

    void on_stickLayerView_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

    void on_deleteStickFigureBtn_clicked();

    void on_moveLayerUpBtn_clicked();

    void on_moveLayerDownBtn_clicked();

private:
    Ui::Widget *ui;
};
#endif // WIDGET_H
