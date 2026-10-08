#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setApplicationName("QtPhoto");
    QApplication::setOrganizationName("QtPhoto");
    Theme::apply(a);

    MainWindow w;
    w.show();
    if (argc > 1)
        w.openFile(QString::fromLocal8Bit(argv[1]));
    return QApplication::exec();
}
