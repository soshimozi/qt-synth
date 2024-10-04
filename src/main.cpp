#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    w.setFixedSize(1024, 768);

    QPalette pal = QPalette();

    // set black background
    // Qt::black / "#000000" / "black"
    pal.setColor(QPalette::Window, Qt::black);
    w.setAutoFillBackground(true);
    w.setPalette(pal);

    w.show();
    return a.exec();
}
