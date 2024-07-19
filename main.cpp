#include "constructar.h"
#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ConstructAR w;
    w.show();

    return a.exec();
}
