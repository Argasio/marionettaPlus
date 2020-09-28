#ifndef MYMAINWINDOW_H
#define MYMAINWINDOW_H

#include <QMainWindow>
#include "myview.h"
#include "widget.h"
#include <QObject>
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


    void exportImageSequence(QString path);
    void exportUsingFFMPEG();
public slots:
    void getAnimationParams(int optionVal);
    void startffmpegRender(QStringList options);
private slots:
    void on_actionsave_triggered();

    void on_actionload_triggered();

    void on_actionsave_animation_triggered();

    void on_actionload_animation_triggered();

    void on_actionexport_as_AVI_file_triggered();

    void on_actionsave_animation_current_triggered();

    void on_actionNew_Blank_Animation_triggered();

    void on_actionAnimation_Options_triggered();

    void on_actionExport_video_using_FFMPEG_triggered();

    void on_actionExport_as_image_sequence_triggered();

private:
    Ui::myMainWindow *ui;

};

#endif // MYMAINWINDOW_H
