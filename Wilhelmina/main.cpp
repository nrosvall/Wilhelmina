#include "Wilhelmina.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Wilhelmina w;
    w.show();
    return a.exec();
}
