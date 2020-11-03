#include "checkupdate.h"
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
    p->start((programFolder.path()+"/sql.exe"));
    if (!p->waitForStarted()){

    }else{
        p->waitForFinished();
        output = (p->readAllStandardOutput());
        QStringList release = output.split(QLatin1Char(','), QString::SkipEmptyParts);
        if(release[0].toInt() == -1){
            return;
        }else{
            if(release[0].toInt() > VERSION){
                goto update;
            }else if(release[0].toInt() == VERSION){
                if(release[1].toInt() > SUBVERSION){
                    goto update;
                }else if(release[1].toInt() == SUBVERSION){
                    if(release[2].toInt() > R){
                        goto update;
                    }
                }
            }
        }

    }
    update:
        emit updatePresent(output);
}
