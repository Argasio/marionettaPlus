#include "checkupdate.h"
#include <QCoreApplication>
#include <QDir>
#include <QProcess>
#define VERSION 1
#define SUBVERSION 1
#define R 1
extern QDir programFolder;
checkUpdate::checkUpdate()
{

}

void checkUpdate::run(){
    QProcess *p = new QProcess(this);
    QStringList paramList;
    QString output;
    QString exePath = QCoreApplication::applicationDirPath();
    QFile::copy(":/version.exe",exePath+"/version.exe");
    p->start(exePath+"/version.exe");
    if (!p->waitForStarted()){

    }else{
        p->waitForFinished();
        output = (p->readAllStandardOutput());

        output.remove("(",Qt::CaseSensitivity::CaseSensitive);
        output.remove(")",Qt::CaseSensitivity::CaseSensitive);
        output.remove("\r\n",Qt::CaseSensitivity::CaseSensitive);
        if(output.length()<3){
            return;
        }
        QStringList release = output.split(QLatin1Char(','), QString::SkipEmptyParts);
        int V = release[0].toInt();
        int SubV = release[1].toInt();
        int Rel = release[2].toInt();
        if(release[0].toInt() == -1){
            return;
        }else{
            if(V> VERSION){
                goto update;
            }else if(V == VERSION){
                if(SubV > SUBVERSION){
                    goto update;
                }else if(SubV == SUBVERSION){
                    if(Rel> R){
                        goto update;
                    }
                }
            }
        }
        update:
            emit updatePresent(output);

    }

}
