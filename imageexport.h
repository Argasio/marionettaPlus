#ifndef IMAGEEXPORT_H
#define IMAGEEXPORT_H

#include "myview.h"

#include <QObject>
#include <QThread>
#define FFMPEG 1
#define AVI 2
#define PREVIEW 3
#define NONE 0
extern myView *V;
class imageExport : public QThread
{
    Q_OBJECT
public:
    explicit imageExport(QObject *parent = nullptr, QString path = "", int option = NONE);
    void render(int option);
signals:
    void imageExportFinished(int option);
public slots:


    // QThread interface
protected:
    virtual void run() override;
};

#endif // IMAGEEXPORT_H
