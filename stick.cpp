#include "stick.h"
#include <QGraphicsItem>

bool onionRender = false;
extern QGraphicsRectItem* myRect;
extern QPixmap * imageDrawBuffer ;
//lo stick si basa fondamentalmente su un QGraphicsLineObject, uno stick alloca una lineobject
stick::stick(QLineF *line)
{
    this->setFlag(QGraphicsItem::ItemIsSelectable,true);
    Pen = QPen();
    Brush = QBrush();
    Brush.setStyle(Qt::SolidPattern);
    highlight = true;
    setLine(line);
    stepchild = false;

}
stick::stick()
{
    this->setFlag(QGraphicsItem::ItemIsSelectable,true);
    //Pen = QPen();
}
stick::stick(stick* S)
{
    this->setFlag(QGraphicsItem::ItemIsSelectable,true);
    Pen = S->Pen;
    highlight = true;
    setLine(&S->myLine);
    stepchild = false;
    this->Z = S->Z;
    this->br = S->boundingRect();
    this->type = S->type;
    if(S->type == IMAGE){
        this->stickImg = new QImage(*S->stickImg);
        imgAngle = S->imgAngle;
        imgWScale = S->imgWScale;
        imgHScale = S->imgHScale;
        imgAngleOffset = S->imgAngleOffset;
        imgOffset = S->imgOffset;
    }
}
stick::~stick(){
    if(type == IMAGE)
        delete stickImg;
}
void stick::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    //Pen.setColor(Qt::black);

    this->setZValue(Z);
    if(!onionRender)
        painter->setPen(Pen);
    else{
        QPen onionSkinPen(Pen);
        onionSkinPen.setColor(Qt::gray);
        painter->setPen(onionSkinPen);
    }
    if(type == stickType::CIRCLE){
        painter->setBrush(Brush);
        painter->drawEllipse(QPointF
                             (0.5*(myLine.p1().x()+myLine.p2().x()),
                              0.5*(myLine.p1().y()+myLine.p2().y())), // Ellipse center = line center
                             myLine.length()*0.5, myLine.length()*0.5); //RX e RY

    }
    else if(type == stickType::IMAGE){
        //QRectF imgRect = calcImgRect(myLine, stickImg->size());
        // ogni immagine la facciamo partire dal punto 0,0
        // la ruotiamo
        // la trasliamo al punto di p1
        // il punto p1 deve essere sempre il topleft corner
        QTransform t;
        QSizeF off;
        QPointF p;
        //vedi quale fra lunghezza o larghezza dell'immaggine èd la più alta per calcolare lóffset per centrare l'immagine su p1
        QImage scaled = calcImg();
        // in caso di onion rendering converti l'immagine in grayscale
        if(onionRender){
            scaled.convertToFormat(QImage::Format_Grayscale8);
        }
        p = QPointF(myLine.p1().x(),myLine.p1().y());
        QPointF p2 = QPointF(-scaled.width()/2,-scaled.height()/2);
        painter->translate(p);
        painter->rotate(imgAngle+imgAngleOffset);
        QPointF p3 = QPointF(imgOffset);
        painter->translate(p3);
        QPixmap pixmap;
        pixmap.convertFromImage(scaled);
        // se siamo in onionskin

        painter->drawPixmap(p2,
                            pixmap);
        painter->translate(-p3.x(),-p3.y());
        painter->rotate(-imgAngle-imgAngleOffset);
        painter->translate(-p.x(),-p.y());

    }
    else if(type == stickType::LINE){
        painter->drawLine(myLine.x1(),myLine.y1(),myLine.x2(),myLine.y2());
        //painter->drawEllipse(myLine.p1().x()-Pen.width()/2,myLine.p1().y()-Pen.width()/2,Pen.width()/2,Pen.width()/2);
       // painter->drawEllipse(myLine.p2().x()-Pen.width()/2,myLine.p2().y()-Pen.width()/2,Pen.width()/2,Pen.width()/2);
        //painter->rotate(imgAngle+imgAngleOffset);
    }
    //Pen.setColor(Qt::red); //draw bounding box
    if(!onionRender){
        if(this->master){ // il master ha entrambe le estremità disegnate
            if(this->highlight)
                painter->setPen(QPen(Qt::green,10));
            else
                painter->setPen(QPen(Qt::darkGreen,10));
            painter->drawEllipse(myLine.p1().x()-5,myLine.p1().y()-5,10,10);
        }
        if(this->parent != nullptr){
            if(this->highlight){
                painter->setPen(QPen(Qt::red,10));
                if(this->stepchild && this->parent->master)
                    painter->setPen(QPen(Qt::green,10));
            }
            else{
                painter->setPen(QPen(Qt::blue,10));
                if(this->stepchild && this->parent->master)
                    painter->setPen(QPen(Qt::darkGreen,10));
            }
            //if(this->parent->master == false)
                painter->drawEllipse(myLine.p1().x()-5,myLine.p1().y()-5,10,10);
        }
        if(this->highlight)
            painter->setPen(QPen(Qt::red,10));
        else
            painter->setPen(QPen(Qt::blue,10));
        painter->drawEllipse(myLine.p2().x()-5,myLine.p2().y()-5,10,10);
        if(this->selected){
            QColor c(Qt::yellow);
            c.setAlphaF(0.75);
            painter->setPen(QPen(c,8));

            painter->drawEllipse(myLine.p2().x()-4,myLine.p2().y()-4,8,8);
            painter->drawEllipse(myLine.p1().x()-4,myLine.p1().y()-4,8,8);

                painter->drawLine(myLine.x1(),myLine.y1(),myLine.x2(),myLine.y2());

        }
        //painter->drawRect(br);
    }
}
// bounding rectangle deve essere sempre "positivo"
QRectF stick::boundingRect() const
{
    return this->br;
}

QRectF calcImgRect(QLineF l,QSizeF s){
    float f = l.length()/s.width();
    QRectF out = QRectF(QPointF                                                      // Upper left corner of rect =
                   (l.p1().x()-abs(l.length())*0.5, //           line center X shifted by minus half line length
                    l.p1().y()-abs(s.height()*f)*0.5), //          line center Y shifted by minus half line length (pushing top left)
                   QPointF(l.p1().x()+abs(l.length())*0.5, // bottom right corner =
                                       l.p1().y()+abs(s.height()*f)*0.5));// line center shifted by adding half line length (pushing bottom right)

    /*
    //questa cerca adattivamente quale sia il punto più in alto a sx
    QRectF out =QRectF(QPointF                                                      // Upper left corner of rect =
                       (0.5*(l.p1().x()+l.p2().x())-abs(l.length())*0.5, //           line center X shifted by minus half line length
                        0.5*(l.p1().y()+l.p2().y())-abs(s.height()*f)*0.5), //
                       QPointF(0.5*(l.p1().x()+l.p2().x())+abs(l.length())*0.5, // bottom right corner =
                                           0.5*(l.p1().y()+l.p2().y())+abs(s.height()*f)*0.5));*/

    /*QRectF out =QRectF(0.5*(l.p1()+l.p2())-QPointF(),
                       QPointF(l.p1().x()+abs(l.dx()), // bottom right corner =
                                           l.p1().y()+(s.height()*f)));*/
    return out;

}

QImage stick::calcImg(){
    QSizeF offsetted;
    if(stickImg->width()>stickImg->height())
        offsetted = QSizeF(myLine.length()*imgWScale, stickImg->height()*myLine.length()/stickImg->width()*imgHScale);
    else
        offsetted = QSizeF(myLine.length()*stickImg->width()*imgWScale/stickImg->height(), myLine.length()*imgHScale);

    return  stickImg->scaled(offsetted.width(),offsetted.height());
}
//funzione per aggiornare il rettangolo di refresh del disegno della scena per l'item grafico
QRectF stick::updateBr(int mode)
{
    QRectF newBr;
    qreal oldP1x;
    qreal oldP1y;
    qreal oldP2x;
    qreal oldP2y;
    qreal newP1x;
    qreal newP1y;
    qreal newP2x;
    qreal newP2y;
    qreal fP1x;
    qreal fP1y;
    qreal fP2x;
    qreal fP2y;

    newBr = QRectF(QPointF                                                      // Upper left corner of rect =
                   (0.5*(myLine.p1().x()+myLine.p2().x())-myLine.length()*0.5, //           line center X shifted by minus half line length
                    0.5*(myLine.p1().y()+myLine.p2().y())-myLine.length()*0.5), //          line center Y shifted by minus half line length (pushing top left)
                   QPointF(0.5*(myLine.p1().x()+myLine.p2().x())+myLine.length()*0.5, // bottom right corner =
                                       0.5*(myLine.p1().y()+myLine.p2().y())+myLine.length()*0.5));     // line center shifted by adding half line length (pushing bottom right)

    if(type == IMAGE && myLine.length()>0){



        QImage scaled = calcImg();
        float d = sqrt(pow(scaled.width(),2)+pow(scaled.height(),2));
        newBr = QRectF(QPointF                                                      // Upper left corner of rect =
                       (myLine.p1().x()-d, //           line center X shifted by minus half line length
                        myLine.p1().y()-d), //          line center Y shifted by minus half line length (pushing top left)
                       QPointF(myLine.p1().x()+d, // bottom right corner =
                                           myLine.p1().y()+d));
    }
    /*
    // 1 Assicurati che a seconda della direzione della linea, il rettangolo abbia come P1 il punto più in alto a sx
    //      e come P2 il punto più in basso a destra
        if(myLine.dy()>0 && myLine.dx()>0)
            newBr = QRectF(myLine.p1(),myLine.p2());
        else if(myLine.dy()>0 && myLine.dx()<=0)
            newBr = QRectF(QPointF(myLine.p2().x(),myLine.p1().y()),
                            QPointF(myLine.p1().x(),myLine.p2().y()));
        else if(myLine.dy()<=0 && myLine.dx()>0)
            newBr = QRectF(QPointF(myLine.p1().x(),myLine.p2().y()),
                            QPointF(myLine.p2().x(),myLine.p1().y()));
        else
            newBr = QRectF(myLine.p2(),myLine.p1());*/
        newBr.setX(newBr.x()-Pen.width());newBr.setY(newBr.y()-Pen.width()); // TODO padding proporzionale alla dimensione della Penna
        newBr.setWidth(newBr.width()+Pen.width());
        newBr.setHeight(newBr.height()+Pen.width());
        this->br.getCoords(&oldP1x,&oldP1y,&oldP2x,&oldP2y);
        // Se stiamo muovendo l'oggetto, teniamo conto anche del precedente bounding rect, e li fondiamo
        // per creare un bounding rect che contenga sia quello nuovo che quello vecchio così da non generare ghosting
        if(mode == 1){
            newBr.getCoords(&newP1x,&newP1y,&newP2x,&newP2y);
            if(oldP1x<=newP1x)
                fP1x = oldP1x;
            else
                fP1x = newP1x;
            if(oldP1y<=newP1y)
                fP1y = oldP1y;
            else
                fP1y = newP1y;
            if(oldP2x>=newP2x)
                fP2x = oldP2x;
            else
                fP2x = newP2x;
            if(oldP2y>=newP2y)
                fP2y = oldP2y;
            else
                fP2y = newP2y;
            newBr = QRectF(QPointF(fP1x,fP1y),QPointF(fP2x,fP2y));
        }

        return newBr;

}
//lo stick misura l'angolo effettivo dell'oggetto linea rispetto al piano di disegno
void stick::calcAngle()
{
    angleBuffer = myLine.angle();//(180/M_PI)*atan2(lineobject.line().dx(),lineobject.line().dy());
}

//aggiorna la linea dello stick
void stick::setLine(QLineF* line)
{
    //lineobject.setLine(*line);
    myLine = *line;
}
void stick::refresh(int mode)
{
    prepareGeometryChange();    // necessario chiamare questa funzione per non lasciare trails ed evitare il ghosting durante la manipolazione e il disegno!
    br = updateBr(mode);
    calcAngle();
    update();
}
//comando per cambiare la rotazione di uno stick
void stick::rotate(QPointF *point)
{

    //step 1 calcola l'angolo con alpha = atan2(dx/dy)
    int dx = static_cast<int>(myLine.p1().x()-point->x());
    int dy = static_cast<int>(-myLine.p1().y()+point->y());
    float DX, DY = 0;
    float angle         = atan2(dx,dy)*180/M_PI;
    bool traslationOnly = false;
    //agisci sull'oggetto linea
    QPointF pBuf        = getP2(&myLine);
    //setta il pivot dello stick
    //progettiamo lóffset degli stick figlio
    QPointF oldEndPos   = myLine.p2();
    // lo stick punta al cursore che lo trascina
    //calcola se stiamo lavorando con l'origine del master stick
    if(this->master &&
            QLineF(*point,myLine.p1()).length()<QLineF(*point,myLine.p2()).length()) // se la selezione è più prossima all'orgine che al secondo punto
    {
        oldEndPos   = myLine.p1();
        DX = oldEndPos.x()-point->x(); //opera una traslazione
        DY = oldEndPos.y()-point->y();
        myLine.translate(-DX,-DY);
        traslationOnly = true;
    }
    else
    {
        myLine.setAngle(-angle-90/*(angle+angleBuffer)*/);
    }
    refresh(1);
    for(int i= 0; i<children.length();i++)
    {
        if(traslationOnly){ // inc aso di sola traslazione dello stick non ricalcolare DX e DY
            children[i]->myLine.translate(-DX,-DY);
        }
        // se non è uno stepchild oppure lo è ma stai ruotando uno stepchild a sua volta
        else if( (children[i]->stepchild == false) || (this->stepchild == true && children[i]->stepchild == true)) // dall'altra estremità del master non spostare gli stick quando il master è ruotato
        {
            //effettua la traslazione dovuta alla rotazione dell'estremo libero
            DX = oldEndPos.x()-myLine.p2().x();
            DY = oldEndPos.y()-myLine.p2().y();
            children[i]->myLine.translate(-DX,-DY);
            qDebug("DX %f, DY %f", DX,DY);
            qDebug("X2= %f, Y2= %f", pBuf.x(),pBuf.y());

        }
        children[i]->refresh(1);
    }
    if(type == IMAGE && !traslationOnly){
        imgAngle = angle;
        //*stickImg = stickImg->transformed(QTransform().rotate(90-angle));
    }

}
// funzione usata per fare le rotazioni dei singoli stick in modalità stickfigure rotation
// angle è il delta di quanto lo vogliamo ruotare
void stick::rotate(float angle){
    // l'angolo viene settato, quindi in realtà andiamo ad aggiungere un delta
    angle = angleBuffer2+angle;
    qDebug(" anglebuf = %f \r\n angle = %f\r\n",angleBuffer2,angle);
    //angleBuffer = angle;
    QPointF oldEndPos = myLine.p2();
    myLine.setAngle(angle);
    float DX = 0;
    float DY = 0;
    DX = oldEndPos.x()-myLine.p2().x(); //traslazione dei nodi figli
    DY = oldEndPos.y()-myLine.p2().y();
    refresh(1);
    for(int i= 0; i<children.length();i++)
    {
        // se non è uno stepchild oppure lo è ma stai ruotando uno stepchild a sua volta
        if( (children[i]->stepchild == false) || (this->stepchild == true && children[i]->stepchild == true)) // dall'altra estremità del master non spostare gli stick quando il master è ruotato
        {
            //effettua la traslazione dovuta alla rotazione dell'estremo libero
            children[i]->myLine.translate(-DX,-DY);
        }
        children[i]->refresh(1);
    }
    if(type == IMAGE){
        imgAngle = angle;
        //*stickImg = stickImg->transformed(QTransform().rotate(90-angle));
    }
}
// this freely elongates sticks
void stick::manipulate(QPointF *point)
{
    // calculate distance between original point and clicked point
    int DX = myLine.p2().x()-point->x();
    int DY = myLine.p2().y()-point->y();
    // set the new point
    myLine.setP2(*point);
    for(int i= 0; i<children.length();i++)
    {
        //effettua la traslazione dovuta alla manipolazione dell'estremo libero
        children[i]->myLine.translate(-DX,-DY);
        children[i]->refresh(1);
    }
    // se stiamo con un immagine ruotala anche
    if(type== stick::IMAGE){
        imgAngle= -atan2(myLine.dx(),myLine.dy())*180/M_PI;
    }
    refresh(0);
}
// è stato rilasciato il mouse termina la rotazione
void stick::endRotation()
{
    // refresha con l'opzione di ridimensionare i box
    refresh(0);
    // anche per tutti i children
    for(int i= 0; i<children.length();i++)
    {
        children[i]->refresh(0);
    }
}
QPointF stick::getP2(QLineF* line)
{
    float x = line->p1().x()+line->length()*cos(line->angle());
    float y = line->p1().y()+line->length()*sin(line->angle());
    return QPointF(x,y);
}
void stick::scale(float scaleFactor){
    /*
    float trueScale = 1+scaleBuffer-scaleFactor;
    float oldLength = myLine.length();
    scaleBuffer = scaleFactor;
    */
    QPointF oldEndPos = myLine.p2();
    float oldLength = myLine.length();
    float trueScale = scaleBuffer*scaleFactor;
    float DX = 0;
    float DY = 0;
    myLine.setLength(trueScale);
    DX = oldEndPos.x()-myLine.p2().x(); //traslazione dei nodi figli
    DY = oldEndPos.y()-myLine.p2().y();
    refresh(1);
    Pen.setWidthF(widthBuffer*scaleFactor);
    for(int i= 0; i<children.length();i++)
    {
        // se non è uno stepchild oppure lo è ma stai ruotando uno stepchild a sua volta
        if( (children[i]->stepchild == false) || (this->stepchild == true && children[i]->stepchild == true)) // dall'altra estremità del master non spostare gli stick quando il master è ruotato
        {
            //effettua la traslazione dovuta alla rotazione dell'estremo libero
            children[i]->myLine.translate(-DX,-DY);
            children[i]->refresh(1);
        }
    }
    qDebug("scale = %f \r\n",trueScale);
}
void sceneRemover(QGraphicsScene *sceneToClear){
    QList<QGraphicsItem*> thingsOnScene = sceneToClear->items( Qt::DescendingOrder);
    for(QGraphicsItem* i: thingsOnScene){
        if(i != myRect)
            sceneToClear->removeItem(i);
    }
}
