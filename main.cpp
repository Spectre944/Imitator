#include "imitator.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Imitator w;
    w.show();
    return a.exec();
}
