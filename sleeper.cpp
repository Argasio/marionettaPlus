#include "sleeper.h"
#include "sleeper.h"
#include "imageexport.h"
extern bool playBack;
extern void sceneRemover(QGraphicsScene *sceneToClear);
extern  bool busyPlay;
bool pausePressed;
int intervalTime = 0;
imageExport *previewGenerator;
mySleeper::mySleeper(myView * V, int time)
{
    v = V;
    counter = 0;
    intervalTime = time;
    previewGenerator = new imageExport(this,"",PREVIEW);
    QObject::connect(previewGenerator,&imageExport::imageExportFinished,this,&mySleeper::startPlaying);
    playBack = true;
    sceneRemover(v->scene());
    previewGenerator->start();


}

void mySleeper::startPlaying()
{
    connect(&timer, SIGNAL(timeout()), SLOT(connected()));
    timer.setSingleShot(false);
    timer.setInterval(intervalTime);
    timer.start();
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
