#ifndef CHECKUPDATE_H
#define CHECKUPDATE_H
#include <QThread>


class checkUpdate : public QThread
{
    Q_OBJECT
public:
    explicit checkUpdate();

signals:
    void updatePresent(const QString &s);

    // QThread interface
protected:
    virtual void run() override;
};
#endif // CHECKUPDATE_H


