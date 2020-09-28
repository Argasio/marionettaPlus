#ifndef FFMPEGEXPORT_H
#define FFMPEGEXPORT_H

#include <QWidget>

namespace Ui {
class ffmpegExport;
}

class ffmpegExport : public QWidget
{
    Q_OBJECT

public:
    explicit ffmpegExport(QWidget *parent = nullptr);
    ~ffmpegExport();

private slots:
    void on_exportVideoBtn_clicked();

    void on_selectFolderBtn_clicked();

    void on_aviRadioButton_clicked();

    void on_mp4RadioBtn_clicked();

    void on_vmwRadioBtn_clicked();

private:
    Ui::ffmpegExport *ui;
 signals:
  void  exportVideo(QStringList options);
};

#endif // FFMPEGEXPORT_H
