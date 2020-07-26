#include "StickFigure.h"
// StickFigure work in the one active scene


StickFigure::~StickFigure()
{
    for(stick* s: stickList)
    {
        scene->removeItem(s);
        delete s;
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
    masterStick =   NULL;
    linkedItem  = item;
    stickFigureIcon = new QIcon();
    iconImg         = new QPixmap(50,50);
}

//inizio disegno di una linea stick
void StickFigure::startDrawing(QPointF *point)
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
    if(masterStick == NULL){
        masterStick = currentStick;
        masterStick->master = true;
    }
    drawCount       = 1; // segnala che un disegno è in atto
    stickBuffer->refresh(0);
}
// aggiorna uno stick il cui disegno è già avviato
void StickFigure::previewDrawing(QPointF *point)
{
    setLineFromPoint(point);
    stickBuffer->refresh(1);
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
        masterStick = NULL;
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
    parentBuffer    = NULL;
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
            parentBuffer    = stickList[idx];
            chosenIdx = idx;
            selectingOrigin = false;
        }
        else if(QLineF(pBuf1,*point).length()<= minDist && stickList[idx]->master)
        {
            parentBuffer    = stickList[idx];
            selectingOrigin = true;
        }
        idx++;
    }
    return chosenIdx;
}
// delete selected stick and all its children, if this is the master stick delete everything
void StickFigure::deleteStick(int idx)
{
    stick *selectedStick        = stickList[idx];   //oggetto stick che contiene l'oggetto grafico
    scene->removeItem(stickList[idx]); //rimuovi l'oggetto dalla scena
    if(stickList[idx]->master)
        this->masterStick = NULL;
    //per ogni figlio dell'oggetto, rimuovi gli oggetti dalla scena e cancellali
    for(stick * s : selectedStick->children){
        int childIndex          = stickList.indexOf(s);
        stick *childStick       = stickList[childIndex];   //oggetto stick che contiene l'oggetto grafico
        scene->removeItem(stickList[childIndex]); //rimuovi l'oggetto dalla scena
        stickList.removeAt(childIndex);
        stick * parent          = selectedStick->parent;
        // rimuovi a ritroso ciascuno stick figlio di quello selezionato dalle listte figli di tutti gli altri sitck presenti genitori di esso
        while(parent != NULL )
        {
            int removefromparentIdx = parent->children.indexOf(childStick);
            parent->children.removeAt(removefromparentIdx);
            parent = parent->parent; // update parent pointer
        }
        delete childStick;
        parentBuffer = NULL;
    }
    // epura lo stick dalla lista sticklist
    stickList.removeAt(idx);
    // rimuovi lo stick selezionato dalla lista dei suoi genitori
    stick * parent = selectedStick->parent;
    while(parent != NULL)
    {
        int removefromparentIdx = parent->children.indexOf(selectedStick);
        parent->children.removeAt(removefromparentIdx);
        parent                  = parent->parent;
    }
    delete selectedStick; // distruggi l'oggetto
    lineBuffer = QLineF(); // svuota linebuffer

}


