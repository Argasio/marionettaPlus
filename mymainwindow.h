#ifndef MYMAINWINDOW_H
#define MYMAINWINDOW_H

#include <QMainWindow>
#include "myview.h"
#include "widget.h"
#include <QObject>
#include <checkupdate.h>
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

    void doConnections();
    void exportImageSequence(QString path, int option);
    void exportUsingFFMPEG();
    void exportAvi();
public slots:
    void getAnimationParams(int optionVal);
    void startffmpegRender(QStringList options);
    void autoSave();
    void updatePresent(QString ver);
    void imageRenderOver(int option);
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

    void on_actionAbout_triggered();


    void on_actionTerms_and_Conditions_triggered();

private:
    Ui::myMainWindow *ui;


    // QWidget interface
protected:
    virtual void closeEvent(QCloseEvent *event) override;
};

#endif // MYMAINWINDOW_H
