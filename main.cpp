/*
*  main.cpp - Top-level Qt 
*  c. ProtoLogic, 2015, chris@protologicnw.com
*
*  Target: Qt Embedded 4.8x under Linux
*
*  Debug under Mac/PC is possible but not all input functions will work.
*
*  License: LGPL (dynamic link to QtEmb libraries)
*
*/

#include <QtGui/QApplication>
#include <QtGui/QWidget>

//! [main]
#include "ui.h"

#ifndef HO
#error
#define HO "unknown"
#endif

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    UI ui;
    ui.show();

    return app.exec();

}
//! [main]
