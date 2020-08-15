#include "sleeper.h"

mySleeper::mySleeper(myView * V, int time)
{
    v = V;
    counter = 0;
    connect(&timer, SIGNAL(timeout()), SLOT(connected()));
    timer.setSingleShot(false);
    timer.setInterval(time);
    timer.start();
}
Q_SLOT void mySleeper::connected()
{
   if (v->myAnimation->frameList.count()>counter){
       v->moveToFrame(v->myAnimation->frameList[counter]);
       timer.start();
       counter++;
   }
   else{
       timer.stop();
       delete this;
   }


   //do some processing
}
