#ifndef SLEEPER_H
#define SLEEPER_H
#include <QThread>
#include <QTimer>
#include "myview.h"
class mySleeper:public QObject
{
    Q_OBJECT
    QTimer timer;
    myView* v;
    int counter = 0;
public:
    mySleeper(myView * V, int time);
    Q_SLOT void connected();

};
#endif // SLEEPER_H
