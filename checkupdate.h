#ifndef CHECKUPDATE_H
#define CHECKUPDATE_H
#include <QThread>


class checkUpdate : public QThread
{
    Q_OBJECT
public:
    checkUpdate();
    void run() override {
        QString result;
        /* ... here is the expensive or blocking operation ... */
        emit resultReady(result);
    }
signals:
    void resultReady(const QString &s);
};
#endif // CHECKUPDATE_H


