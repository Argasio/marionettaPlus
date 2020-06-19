#ifndef MYRECT_H
#define MYRECT_H

#include <QObject>
#include <QtDebug>
#include <QGraphicsItem>
class myRect : public QObject
{
    Q_OBJECT
public:
    explicit myRect(QObject *parent = nullptr);

signals:

};

#endif // MYRECT_H
