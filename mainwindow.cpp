
#include "mainwindow.h"

// constructor / destructor
mainwindow::mainwindow(QWidget *parent)
    : QWidget(parent)
    { // constructor

    // create menu bar
    menuBar = new QMenuBar(this);

    // create file menu
    fileMenu = menuBar->addMenu("&File");

    // create the action
    actionQuit = new QAction("&Quit", this);


    // add the item to the menu
    fileMenu->addAction(actionQuit);

    // create the window layout
    windowLayout = new QBoxLayout(QBoxLayout::TopToBottom, this);

    // create main widget
    cubeWidget = new widget(this);
    windowLayout->addWidget(cubeWidget);

    // create Y slider
    nVerticesSliderY = new QSlider(Qt::Horizontal);
	// sets Y slider min and max
    nVerticesSliderY->setMinimum(-50);
    nVerticesSliderY->setMaximum(50);
    windowLayout->addWidget(nVerticesSliderY);
	// create Z slider
    nVerticesSliderZ = new QSlider(Qt::Horizontal);
	// sets Z slider min and max
    nVerticesSliderZ->setMinimum(-50);
    nVerticesSliderZ->setMaximum(50);
    windowLayout->addWidget(nVerticesSliderZ);


    ptimer = new QTimer(this);

    ptimer->start(33.333); //30fps

    connect(ptimer, SIGNAL(timeout()),  cubeWidget, SLOT(updateAngle())); // Refreshes exreen
    connect(nVerticesSliderY, SIGNAL(valueChanged(int)),  cubeWidget, SLOT(updateY(int))); // Rotates along y axis
    connect(nVerticesSliderZ, SIGNAL(valueChanged(int)),  cubeWidget, SLOT(updateZ(int))); // Rotates along z axis

    } // constructor

mainwindow::~mainwindow()
    { // destructor
    delete ptimer;
    delete nVerticesSliderY;
    delete nVerticesSliderZ;
    delete cubeWidget;
    delete windowLayout;
    delete actionQuit;
    delete fileMenu;
    delete menuBar;
    } // destructor

// resets all the interface elements
void mainwindow::ResetInterface()
    { // ResetInterface()

      nVerticesSliderY->setMinimum(-3);
      nVerticesSliderY->setMaximum(3);
      nVerticesSliderZ->setMinimum(-3);
      nVerticesSliderZ->setMaximum(3);

    // now force refresh
    cubeWidget->update();
    update();
    } // ResetInterface()
