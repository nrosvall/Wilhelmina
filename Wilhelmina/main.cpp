#include "Wilhelmina.h"
#include <QtWidgets/QApplication>
#include "IdleFilter.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Wilhelmina w;


    IdleFilter* idleFilter = new IdleFilter(&a, 180000);
    idleFilter->setClient(&w);

    a.installEventFilter(idleFilter);

    QObject::connect(idleFilter, SIGNAL(userInactive()),
        &w, nullptr);

    w.show();

    return a.exec();
}
