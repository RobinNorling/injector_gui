#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowFlags(w.windowFlags() | Qt::WindowType::Tool | Qt::WindowStaysOnTopHint);
    w.show();
    return a.exec();
}
