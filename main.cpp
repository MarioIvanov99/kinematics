#include <QApplication>
#include <QVBoxLayout>
#include "mainwindow.h"

int main(int argc, char *argv[])
    { // main()
    // create the application

    QApplication app(argc, argv);

    // create a master widget
        mainwindow *window = new mainwindow(NULL);

    // resize the window
    window->resize(512, 612);

    // show the label
    window->show();

    // start it running
    app.exec();

    // clean up
    //	delete controller;

    delete window;

    // return to caller
    return 0;
    } // main()
