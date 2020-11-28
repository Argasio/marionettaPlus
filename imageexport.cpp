#include "imageexport.h"
#include <frame.h>
#include <QImage>
#include <QPainter>

extern int W;
extern int H;
extern QGraphicsRectItem* myRect;
QString exportPath;
int renderOption = NONE;
imageExport::imageExport(QObject *parent, QString path, int option)
{
    exportPath = path;
    renderOption = option;
}
extern bool renderFlag;
void imageExport::render(int option)
{
    for(Frame* f :V->myAnimation->frameList){
        QGraphicsScene renderScene; //scena fittizia
        QImage *renderImg = new QImage(W,H,QImage::Format_ARGB32); //immagine temporanea
        QPainter painter(renderImg); // painter che esegue il drawing sull immagine
        painter.setRenderHints(QPainter::Antialiasing|QPainter::SmoothPixmapTransform);
        renderScene.setSceneRect(myRect->rect());
        // ora clona tutti gli stick nello stickfigure in una tempList, aggiungili alla scena fittizia
        QList<stick*> tempList;

        for(stick* s: f->totalSticks){
            stick* clone = new stick(s);
            tempList.append(clone);
            renderScene.addItem(clone);
        }

        //disegna sull' immagine e salvala
        renderImg->fill(Qt::white);
        painter.setBackground(QBrush(QColor(Qt::white)));
        renderScene.render(&painter);
        QString itemPath = exportPath;
        itemPath.insert(exportPath.lastIndexOf("."),"_"+QString::number(f->frameNumber));
        renderImg->save(itemPath);
        painter.end();
        delete renderImg;
        renderScene.clear();
        //...add all other video frames here
    }


    emit imageExportFinished(renderOption);
}

void imageExport::run()
{
    if(renderOption != PREVIEW){
        renderFlag = true;
        render(renderOption);
        renderFlag = false;
    }else{
        V->preparePreview();

    }
    emit imageExportFinished(renderOption);
}
