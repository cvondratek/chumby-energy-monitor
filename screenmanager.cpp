/*
*  screenmanager.cpp - UI screen manager / customization layer
*  c. ProtoLogic, 2014, chris@protologicnw.com
*
*  Target: Qt Embedded 4.8x under Linux, AM335x flavor
*  Hardware requirements: SGX accelerated gfx & psoctouch input layer
*
*  License: LGPL (dynamic link to QtEmb libraries)
*
*/

#include <QtGui>
#include <QTimer>

#include "screenmanager.h"

/*
"DINOT"
"fixed"
"helvetica"
"micro"
"Open Sans"
"unifont"
*/
#define FONTSZ_SMALL QFont("Open Sans", 10)
#define FONTSZ_NORMAL QFont("Open Sans", 26)
#define FONTSZ_LARGE  QFont("Open Sans", 36)
#define FONTSZ_XLARGE  QFont("DINOT", 400, QFont::Bold)
#define FONTSZ_CLOCK  QFont("Open Sans", 64, QFont::Bold)

#define UITHEME_LIGHT false
#define UITHEME_DARK  true

ScreenManager::ScreenManager(QWidget* parent)
    : QMainWindow()
{

    myParent=parent;

    setFixedSize(320, 240);
    menuBar()->hide();
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);

    versionBanner= new QLabel(this);
    versionBanner->setGeometry(QRect(QPoint(0,255),QSize(200,15)));
    versionBanner->setAlignment(Qt::AlignLeft);
    versionBanner->setFont(QFont("Open Sans", 10));
    versionBanner->setObjectName("versionBanner");
    versionBanner->setText(" KidClock - " __DATE__ );

    debugBanner = new QLabel(this);
    debugBanner->setGeometry(QRect(QPoint(200,255),QSize(200,15)));
    debugBanner->hide();
    debugBanner->setAlignment(Qt::AlignRight);
    debugBanner->setFont(QFont("Open Sans", 10, QFont::Bold));
    debugBanner->setObjectName("debugBanner");
    debugBanner->setText(" DEBUG BUILD ");

    QFile qss(":/qss/MainApp.qss");
    if (qss.open(QIODevice::ReadOnly) == false) {
        qDebug() << "Unable to open app stylesheet!";
    }
    else {
        AppStyleSheet = QLatin1String(qss.readAll());
        qss.close();
        this->setStyleSheet(AppStyleSheet);
    }

    QStringList list;
    list << "DINOT-CondBold.otf";
    int fontID(-1);
    for (QStringList::const_iterator constIterator = list.constBegin(); constIterator != list.constEnd(); ++constIterator) {
        QFile res(":/fonts/" + *constIterator);
        if (res.open(QIODevice::ReadOnly) == false) {
                qDebug() << "Unable to open font file: " << (QString) *constIterator;
        } else {
            fontID = QFontDatabase::addApplicationFontFromData(res.readAll());
            if (fontID == -1) {
                qDebug() << "Unable to load font: " << (QString) *constIterator;
            }
        }
    }

#if 1
    QFontDatabase *fdb = new QFontDatabase ;
    QStringList fam = fdb->families(QFontDatabase::Latin);
    qDebug() << "Available font families" ;
    for(int i=0 ; i< fam.count(); i++){
        qDebug() << fam[i] ;
    }
#endif

    //set initial properties on UI
    setProperty("darkUI",UITHEME_DARK); //select dark UI theme, initially

    //for maximum performance, create all UI elements w/ hidden visibility
    createElements();
}


void ScreenManager::ShowScreen(ScreenManager::SCREENS screen)
{
    //hide existing elements
    foreach(QWidget* obj, visibleObjects)
        obj->hide();
    visibleObjects.clear();

    switch(screen)
    {
    case ERROR_FATAL:
        hlprScreenErrorFatal();
        break;
    case LOGO:
        hlprScreenLogo();
        break;
    case CLOCK:
        hlprScreenClock();
        break;
    }
}


#define DIGIT_WIDTH 60
#define DIGIT_HEIGHT 120
#define DIGIT_SPACING 2
#define CLOCK_LEFT 15
#define CLOCK_TOP  80
#define COLON1_WIDTH 10
#define COLON2_WIDTH 10

void ScreenManager::SetupClockDigit(QPushButton* c, int pos)
{
    //c->hide();
    c->setProperty("darkUI",UITHEME_DARK);
    c->setFont(FONTSZ_CLOCK);
    if(pos<2) //hours
        c->setGeometry(QRect(QPoint(CLOCK_LEFT+(pos*(DIGIT_WIDTH+DIGIT_SPACING)),CLOCK_TOP),QSize(DIGIT_WIDTH,DIGIT_HEIGHT)));
    else if(pos<4) //mins
        c->setGeometry(QRect(QPoint(COLON1_WIDTH+CLOCK_LEFT+(pos*(DIGIT_WIDTH+DIGIT_SPACING)),CLOCK_TOP),QSize(DIGIT_WIDTH,DIGIT_HEIGHT)));
    else if(pos<6) //secs
        c->setGeometry(QRect(QPoint(COLON1_WIDTH+COLON2_WIDTH+CLOCK_LEFT+(pos*(DIGIT_WIDTH+DIGIT_SPACING)),CLOCK_TOP),QSize(DIGIT_WIDTH,DIGIT_HEIGHT)));
    else if(pos==6) //colon1
        c->setGeometry(QRect(QPoint(CLOCK_LEFT+(2*(DIGIT_WIDTH+DIGIT_SPACING)),CLOCK_TOP),QSize(COLON1_WIDTH,DIGIT_HEIGHT)));
    else //colon2
        c->setGeometry(QRect(QPoint(CLOCK_LEFT+COLON1_WIDTH+(4*(DIGIT_WIDTH+DIGIT_SPACING)),CLOCK_TOP),QSize(COLON2_WIDTH,DIGIT_HEIGHT)));

    if(pos>5)
    {
        c->setObjectName("clockColon");
    }
    else
    {
        c->setObjectName("clockDigit");
    }

}

/* preload all elements and stash the offscreen...*/
void ScreenManager::createElements()
{
    QWidget* parent=this;

    //pixmaps
    imgLogo = new QLabel(parent);
    imgLogo->hide();
    imgLogo->setGeometry(QRect(QPoint(0,0),QSize(320,240)));
    QImage logo(":/pixmaps/logo.png");
    imgLogo->setPixmap(QPixmap::fromImage(logo));

    //labels
    scrText = new QLabel(parent);
    scrText->hide();
    scrText->setProperty("darkUI",UITHEME_DARK);
    scrText->setFont(FONTSZ_NORMAL);
    scrText->setAlignment(Qt::AlignCenter);
    scrText->setObjectName("scrText");

    boldHeader = new QLabel(parent);
    boldHeader->hide();
    boldHeader->setProperty("darkUI",UITHEME_DARK);
    boldHeader->setFont(FONTSZ_CLOCK);
    boldHeader->setAlignment(Qt::AlignCenter);
    boldHeader->setObjectName("boldHeader");

    errorText = new QLabel(parent);
    errorText->hide();
    errorText->setFont(FONTSZ_SMALL);
    errorText->setAlignment(Qt::AlignLeft);
    errorText->setObjectName("errorText");

    /*
    for(int i=0; i<6; i++)
    {
        SetupClockDigit(&clockA[i],i);
        SetupClockDigit(&clockB[i],i);
        clockA[i].setText("1");
        clockB[i].setText("2");
        clockB[i].hide();
    }

    SetupClockDigit(&clockColon1,6);
    SetupClockDigit(&clockColon2,7);
    clockColon1.setText(":");
    clockColon2.setText(":");
*/
    clockState=0;

}

void ScreenManager::makeElementVisible(QWidget* el)
{
    el->show();
    visibleObjects.append(el);
}

void ScreenManager::hlprScreenErrorFatal()
{
    errorText->setGeometry(QRect(QPoint(0,50),QSize(480,50)));
    makeElementVisible(errorText);
}

void ScreenManager::hlprScreenLogo()
{
    makeElementVisible(imgLogo);
}

void ScreenManager::hlprScreenClock()
{
    boldHeader->setGeometry(QRect(QPoint(0,21),QSize(320,200)));
    boldHeader->setAlignment(Qt::AlignCenter);
    boldHeader->setText("00:00");
    makeElementVisible(boldHeader);

}

void ScreenManager::hlprUITheme(bool dark)
{
    if(dark) {
        this->setProperty("darkUI",UITHEME_DARK);
        scrText->setProperty("darkUI",UITHEME_DARK);
        boldHeader->setProperty("darkUI",UITHEME_DARK);
    }
    else {
        this->setProperty("darkUI",UITHEME_LIGHT);
        scrText->setProperty("darkUI",UITHEME_LIGHT);
        boldHeader->setProperty("darkUI",UITHEME_LIGHT);
    }

    this->style()->unpolish(this);
    this->style()->polish(this);
    this->update();

    scrText->style()->unpolish(scrText);
    scrText->style()->polish(scrText);
    scrText->update();

    boldHeader->style()->unpolish(boldHeader);
    boldHeader->style()->polish(boldHeader);
    boldHeader->update();

}

