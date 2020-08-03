#include "widget.h"
#include <QApplication>
#include "mymainwindow.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    myMainWindow w;
    Widget* myW = new Widget();
    w.setCentralWidget(myW);
    w.show();
    return a.exec();
}
