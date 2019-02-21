#include "mainwindow.h"
#include <QApplication>
#include<iostream>
#include "graph.h"
using namespace std;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();

}
