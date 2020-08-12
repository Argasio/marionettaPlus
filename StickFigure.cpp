#include "StickFigure.h"

#include <QDebug>
#include <QFile>
// StickFigure work in the one active scene

extern bool undoFlag;
StickFigure::~StickFigure()
{
    if(!undoFlag){
        for(stick* s: stickList)
        {
            scene->removeItem(s);
            delete s;
        }
    }
    delete iconImg;
    delete stickFigureIcon;
}
// questa funzione aggiorna l'iconea del livello
void StickFigure::updateIcon()
{
    //crea una scena fittizia, falla bianca e delle stesse dimensioni di quella principale
    QGraphicsScene icoScene;
    iconImg->fill(Qt::white);
    icoScene.setBackgroundBrush(QBrush(QColor(Qt::white)));
    icoScene.setSceneRect(scene->sceneRect());
    // ora clona tutti gli stick nello stickfigure in una tempList, aggiungili alla scena fittizia
    QList<stick*> tempList;
    for(stick* S: stickList){
        stick* clone = new stick(S);
        tempList.append(clone);
        icoScene.addItem(clone);
    }
    // crea un painter che disegni sull'immagine
    QPainter painter(iconImg);
    // renderizza dalla scena sull'immagine mediante il painter
    icoScene.render(&painter);
    // imposta l'iconea
    *stickFigureIcon = QIcon(*iconImg);
    linkedItem->setIcon(*stickFigureIcon);
    // cancella gli elementi costruiti
    for(stick* S: tempList){
        delete S;
    }
}
StickFigure::StickFigure(QListWidgetItem* item)
{
    p0          =  QPointF(0,0);
    p1          =  QPointF(0,0);
    lineBuffer  =  QLineF(p0,p1);
    drawCount   =  0;
    masterStick =   nullptr;
    linkedItem  = item;
    stickFigureIcon = new QIcon();
    iconImg         = new QPixmap(50,50);
    masterStick = nullptr;
}
StickFigure::StickFigure()
{
    p0          =  QPointF(0,0);
    p1          =  QPointF(0,0);
    lineBuffer  =  QLineF(p0,p1);
    drawCount   =  0;
    masterStick =   nullptr;
    stickFigureIcon = new QIcon();
    iconImg         = new QPixmap(50,50);
    masterStick = nullptr;
}
//inizio disegno di una linea stick
void StickFigure::startDrawing(QPointF *point, QPen pen)
{
    if(stickList.isEmpty()) // se è il primo stick dello stickfigure
    {
        p0 = *point;
    }
    else //se ci sono altri stick in questo stickfigure i dalla estremità più vicinaz al click
    {
        p0 = selectOrigin(point); //seleziona punto di origine
        parentBuffer = stickList[selectStick(point)]; //seleziona genitore (potremmo già farlo al rigo precedente visto che la funzione viene chiamata)
    }

    //crea e alloca l'oggetto stick associato
    lineBuffer      = QLineF(p0,p0); // all'inizio la linea ha lunghezza zero
    stick *stk      = new stick(&lineBuffer);
    //assegna l'ordine di profondità
    stk->Z = baseZ+stickList.count()/1000;
    stickBuffer     = stk;
    currentStick    = stk;
    // seè il primo stick ad essere disegnato
    if(masterStick == nullptr){
        masterStick = currentStick;
        masterStick->master = true;
        masterStick->parent = nullptr;
    }
    drawCount       = 1; // segnala che un disegno è in atto
    stickBuffer->Pen = pen;
    stickBuffer->refresh(0);
}
// aggiorna uno stick il cui disegno è già avviato
void StickFigure::previewDrawing(QPointF *point)
{
    setLineFromPoint(point);
    stickBuffer->refresh(1);
}
void StickFigure::refresh()
{
    for(stick* s:stickList){
        s->refresh(0);
    }
}

void StickFigure::setLineFromPoint(QPointF *point)
{
    p1              = *point;
    lineBuffer      = QLineF(p0,p1);
    // lo stick si fonda sulla linea che connette starting point e endpoint
    stickBuffer->setLine(&lineBuffer);
}

void StickFigure::highlight(bool setting)
{
    for(stick* s:this->stickList)
    {
        s->highlight = setting;
        s->refresh(0);
    }
}
void StickFigure::updateZ(void){
    for(stick* S : stickList){
        S->Z = baseZ;
    }
}
void StickFigure::endDrawing(QPointF *point)
{
    setLineFromPoint(point);
    drawCount = 0;                  //segnala la fine
    stickList.append(stickBuffer);  //disegno finito, aggiungilo alla lista
    // assegna l'elemento padre
    stickBuffer->parent = parentBuffer;
    stickBuffer->refresh(0);
    // INSERISCILO COME FIGLIO DI TUTTA LA GERARCHIA PRECEDENTE
    while(parentBuffer){
        parentBuffer->children.append(stickBuffer);
        if(selectingOrigin || parentBuffer->stepchild){ // children of a stepchild is also a stepchild!
            stickBuffer->stepchild = true;
        }
        parentBuffer = parentBuffer->parent;
    }
}
void StickFigure::cancelDrawing()
{
    if(stickBuffer==masterStick)
        masterStick = nullptr;
    delete stickBuffer;
    lineBuffer  = QLineF();
    drawCount   = 0;

}

//scegli come origine lo stick più vicino al mouse fra gli stick dello stickfigure
QPointF StickFigure::selectOrigin( QPointF * point)
{
    //seleziona lo stick più vicino
    int idx = selectStick(point);
    // se è il master stick
    if(stickList[idx] == masterStick)
    {
        //controlla quale estremità è la più vicina e aseconda di quale essa sia restituisci il punto
        if(QLineF(stickList[idx]->myLine.p2(),*point).length() >
               QLineF(stickList[idx]->myLine.p1(),*point).length() )
        {
            // segnala che stiamo toccando l'orgine
            selectingOrigin = true;
            return stickList[idx]->myLine.p1();

        }
        else{
            return stickList[idx]->myLine.p2();
        }
    }
    // se non è il master stick ignora P1
    else{
        stickList[idx]->myLine.p2();
        return stickList[idx]->myLine.p2();
    }
}
int StickFigure::selectStick( QPointF * point)
{
    int idx         = 0;
    float  minDist  = 0;
    int chosenIdx = 0;
    QPointF pBuf1,pBuf2,pBufOut;
    //inizializziamo a partire dall'origine del masterStick
    pBufOut = masterStick->myLine.p1();
    minDist = QLineF(pBufOut,*point).length();
    while(idx< stickList.length())
    {
        // i punti inizio-fine dello stick corrente
        pBuf1 = stickList[idx]->myLine.p1();
        pBuf2 = stickList[idx]->myLine.p2();
        // se la distanza è più bassa di quella minima registrata dal luogo del mouse
        if(QLineF(pBuf2,*point).length()<= minDist)
        {
            // quello è l'origine
            pBufOut         = pBuf2;
            minDist         = QLineF(pBuf2,*point).length();
            chosenIdx = idx;
            selectingOrigin = false;
        }
        else if(QLineF(pBuf1,*point).length()<= minDist && stickList[idx]->master)
        {
            selectingOrigin = true;
        }
        idx++;
    }
    return chosenIdx;
}
// delete selected stick and all its children, if this is the master stick delete everything
void StickFigure::deleteStick(int idx)
{
    stick *selectedStick = stickList[idx];   //oggetto stick che contiene l'oggetto grafico
    scene->removeItem(stickList[idx]); //rimuovi l'oggetto dalla scena
    if(stickList[idx]->master)
        this->masterStick = nullptr;
    //per ogni figlio dell'oggetto, rimuovi gli oggetti dalla scena e cancellali
    for(stick * s : selectedStick->children){
        int childIndex          = stickList.indexOf(s);
        stick *childStick       = stickList[childIndex];   //oggetto stick che contiene l'oggetto grafico
        scene->removeItem(stickList[childIndex]); //rimuovi l'oggetto dalla scena
        stickList.removeAt(childIndex);
        stick * parent          = selectedStick->parent;
        // rimuovi a ritroso ciascuno stick figlio di quello selezionato dalle listte figli di tutti gli altri sitck presenti genitori di esso
        while(parent != nullptr )
        {
            int removefromparentIdx = parent->children.indexOf(childStick);
            parent->children.removeAt(removefromparentIdx);
            parent = parent->parent; // update parent pointer
        }
        delete childStick;
        parentBuffer = nullptr;
    }
    // epura lo stick dalla lista sticklist
    stickList.removeAt(idx);
    // rimuovi lo stick selezionato dalla lista dei suoi genitori
    stick * parent = selectedStick->parent;
    while(parent != nullptr)
    {
        int removefromparentIdx = parent->children.indexOf(selectedStick);
        parent->children.removeAt(removefromparentIdx);
        parent                  = parent->parent;
    }
    delete selectedStick; // distruggi l'oggetto
    lineBuffer = QLineF(); // svuota linebuffer

}
// serializzatori e deserializzatori della classe stick
QDataStream & operator<< (QDataStream& stream, const stick& myStick){
    //stream<<myStick.Z;
    stream<<myStick.type;
    stream<<myStick.stepchild;
    stream<<myStick.myLine;
    stream<<myStick.parentIdx;
    stream<<myStick.Pen;
    stream<<myStick.master;
    return stream;
}
QDataStream & operator>> (QDataStream& stream, stick& myStick){
    //stream>>myStick.Z;
    stream>>myStick.type;
    stream>>myStick.stepchild;
    stream>>myStick.myLine;
    stream>>myStick.parentIdx;
    stream>>myStick.Pen;
    stream>>myStick.master;
    return stream;
}
// serializzatore per stickfigure
QDataStream & operator<< (QDataStream& stream, const StickFigure& myStickFigure){
    // per prima cosa registra il numero di stick che lo compongono

    stream<<myStickFigure.stickList.count();
    stream<<myStickFigure.name;
    // per ogni stick nella lista scrivi l'indice del genitore e poi serializza il tutto
    for(stick* s:myStickFigure.stickList){
        if(!s->master)
        {
            s->parentIdx = myStickFigure.stickList.indexOf(s->parent);
        }
        else{
            s->parentIdx = -1;
        }
        stream<<*s;
    }

}
// deserializza lo stickfigure
QDataStream & operator>> (QDataStream& stream,StickFigure& myStickFigure){

    int max = 0;
    stream>>max; // quanti stick possiede?
    stream>>myStickFigure.name;
    myStickFigure.stickList.clear();
    // per ogni stick popola la lista sticklist
    for(int i = 0; i<max; i++){
        myStickFigure.currentStick = new stick(); // prepara il contenitore
        myStickFigure.stickList.append(myStickFigure.currentStick);
        stream>>*myStickFigure.currentStick; // "idrata il contenitore" con i dati nel file
        myStickFigure.currentStick->Z = myStickFigure.baseZ+i*0.001;
    }
    // ricostruisci le gerarchie di parentela tramite gli indici
    for(stick * s:myStickFigure.stickList){
        if(s->parentIdx>=0){
            // trova il genitore e aggiungi alla lista dei figli del genitore lo stick corrente
            s->parent = myStickFigure.stickList[s->parentIdx];
            stick * currentParent = s->parent;
            // procedi a ritroso per i genitori del genitore
            while(currentParent != nullptr){
                currentParent->children.append(s);
                if(currentParent->parentIdx>= 0 )
                    currentParent = myStickFigure.stickList[currentParent->parentIdx];
                else // arrivati allo stickmaster vai al prossimo
                    break;
            }
        }
        else{ // se stiamo considerando lo stickmaster
            s->parent = nullptr;
        }
        if(s->master)
            myStickFigure.masterStick = s;
        if(!undoFlag)
            myStickFigure.scene->addItem(s);
        s->refresh(0);
    }
    if(myStickFigure.stickList.count()>0)
        myStickFigure.currentStick = myStickFigure.stickList[0];
    if(!undoFlag)
        myStickFigure.updateIcon();
    return stream;
}
// salva sul percorso impostato i dati serializzati dello stickfigure usando il datastream
void StickFigure::saveStickFigure(QString name)
{

    QFile file(name);
    if(!file.open(QIODevice::WriteOnly))
        return;
    QDataStream out(&file);
    out << *this;
    file.close();
}
// attraverso il percorso file, carica un data stream su cui caricare i dati salvati
// chiama il deserializzatore per stickfigures per interpretare lo stream
void StickFigure::loadStickFigure(QString name)
{

    QFile file(name);
    if(!file.open(QIODevice::ReadOnly))
        return;
    QDataStream out(&file);
    out >> *this;

    file.close();
    qDebug("load finished");

}

QDataStream & operator>> (QDataStream& stream, stick& myStick);
