#ifndef STICKLIB_H
#define STICKLIB_H

#include <QWidget>

namespace Ui {
class stickLib;
}

class stickLib : public QWidget
{
    Q_OBJECT

public:
    explicit stickLib(QWidget *parent = nullptr);
    ~stickLib();

private:
    Ui::stickLib *ui;
};

#endif // STICKLIB_H
