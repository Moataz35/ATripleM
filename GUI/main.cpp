#include "SideBar.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SideBar SB;
    SB.show();
    return QApplication::exec();
}
