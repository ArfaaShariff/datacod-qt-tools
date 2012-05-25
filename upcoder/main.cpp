#include <QtGui/QApplication>
#include "mainwindow.h"
#include <QTranslator>
#include <QLocale>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator myTranslator;
    myTranslator.load("upcoder_"+QLocale::system().name(), ":/translations");
    a.installTranslator(&myTranslator);

    QTranslator qtTranslator;
    qtTranslator.load("qt_"+QLocale::system().name(), ":/translations");
    a.installTranslator(&qtTranslator);

    MainWindow w;
    w.show();
    return a.exec();
}
