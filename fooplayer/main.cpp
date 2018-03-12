#include "player.h"
#include <QApplication>
#include <QDesktopWidget>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Player w;
    QDesktopWidget dw;
    QRect mainScreenSize = dw.availableGeometry(dw.primaryScreen());
    int x = mainScreenSize.width()*0.7;
    int y = mainScreenSize.height()*0.7;
    w.resize(x,y);
    w.show();

    return a.exec();
}
