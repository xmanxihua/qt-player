#include "xplay.h"
#include <QApplication>
#include "xffmpeg.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    XPlay w;
    w.show();

    return a.exec();
}
