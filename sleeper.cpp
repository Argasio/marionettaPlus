#include "sleeper.h"
#include "sleeper.h"
extern bool playBack;
extern void sceneRemover(QGraphicsScene *sceneToClear);
extern  bool busyPlay;
bool pausePressed;
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
void mySleeper::stop(){
    pausePressed = true;
}
Q_SLOT void mySleeper::connected()
{

   if (v->myAnimation->frameList.count()>counter && pausePressed == false){
       //v->moveToFrame(v->myAnimation->frameList[counter]);
       v->displayPreview(counter);
       timer.start();
       counter++;
   }
   else{
       playBack = false;
       timer.stop();
       //v->scene()->clear();
       int i = 0;
       for(QGraphicsPixmapItem * item:v->previewList){
           if(i<=counter)
                v->scene()->removeItem(item);
           delete item;
       }
       v->previewList.clear();
       v->moveToFrame(v->myAnimation->frameList[counter-1]);
       busyPlay = false;
       delete this;
       pausePressed = false;
   }


   //do some processing
}
