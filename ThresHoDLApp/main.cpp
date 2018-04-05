#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    // Set differently for Desktop
    w.showMaximized();
    w.show();

    return a.exec();
}
