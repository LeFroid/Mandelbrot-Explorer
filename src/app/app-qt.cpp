#include <QApplication>

#include "window.h"

int main(int argc, char **argv)
{
    QApplication a(argc, argv);
    Window win;
    win.show();
    return a.exec();
}
