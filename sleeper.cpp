#include "sleeper.h"
extern bool playBack;
extern void sceneRemover(QGraphicsScene *sceneToClear);
mySleeper::mySleeper(myView * V, int time)
{
    v = V;
    counter = 0;
    connect(&timer, SIGNAL(timeout()), SLOT(connected()));
    timer.setSingleShot(false);
    timer.setInterval(time);
    timer.start();

    playBack = true;
    sceneRemover(v->scene());
    v->preparePreview();

}
Q_SLOT void mySleeper::connected()
{

   if (v->myAnimation->frameList.count()>counter){
       //v->moveToFrame(v->myAnimation->frameList[counter]);
       v->displayPreview(counter);
       timer.start();
       counter++;
   }
   else{
       playBack = false;
       timer.stop();
       //v->scene()->clear();
       for(QGraphicsPixmapItem * item:v->previewList){
           v->scene()->removeItem(item);
            delete item;
       }
       v->previewList.clear();
       v->moveToFrame(v->myAnimation->frameList[0]);
       delete this;
   }


   //do some processing
}
