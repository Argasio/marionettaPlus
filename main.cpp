#include "widget.h"
#include <QApplication>
#include "mymainwindow.h"
Widget *myWidget;
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    myMainWindow w;
    Widget* myW = new Widget();
    myWidget = myW;
    w.setCentralWidget(myW);
    w.showMaximized();
    w.view = myW->view;
    w.centralW = myW;
    w.doConnections();
    return a.exec();
}
