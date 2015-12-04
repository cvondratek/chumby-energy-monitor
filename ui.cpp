/*
*  ui.cpp - Main UI View 
*  c. ProtoLogic, 2015, chris@protologicnw.com
*
*  Target: Qt Embedded 4.8x under Linux
*
*  Debug under Mac/PC is possible but not all input functions will work.
*
*  License: LGPL (dynamic link to QtEmb libraries)
*
*/
#include <QtGui>
#include <QPainter>
#include <QFile>
#include <QTimer>
#include <QDateTime>
#include <QApplication>
#include <QDesktopWidget>
#include <QMouseEvent>
#include <QScreen>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QUrl>
#include <QtXml/QXmlReader>
#include <QWSServer>

#include "ui.h"

#define APPREV "A1"

#define FONTSZ_SMALLEST QFont("Open Sans", 6)
#define FONTSZ_SMALL QFont("Open Sans", 8)
#define FONTSZ_NORMAL QFont("Open Sans", 14)
#define FONTSZ_LARGE  QFont("Open Sans", 24)
#define FONTSZ_XLARGE  QFont("Open Sans", 36, QFont::Bold)
#define FONTSZ_CLOCK  QFont("Open Sans", 14)

UI::UI() : QMainWindow()
{
    qDebug("****ChumbyEntry_" APPREV " - " __DATE__ ", " __TIME__ " ["  HO "]****" );

    //get environment.  These are pulled from nvram by the calling script just prior to launching the Qt app
    //QString username;
    //username = QString::fromUtf8(qgetenv("USERNAME"));
    //qDebug() << "Environment:\n\tUSERNAME:" << qPrintable(username);

    setFixedSize(320, 240);
    //menuBar()->hide();
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);

    QFile qss(":/qss/MainApp.qss");
    if (qss.open(QIODevice::ReadOnly) == false) {
        qDebug() << "Unable to open app stylesheet!";
    }
    else {
        AppStyleSheet = QLatin1String(qss.readAll());
        qss.close();
        this->setStyleSheet(AppStyleSheet);
    }

    versionBanner= new QLabel(this);
    versionBanner->setGeometry(QRect(QPoint(0,225),QSize(160,10)));
    versionBanner->setAlignment(Qt::AlignLeft);
    versionBanner->setFont(FONTSZ_SMALLEST);
    versionBanner->setObjectName("versionBanner");
    versionBanner->setText(" ChumbyEntry - " __DATE__ );
    versionBanner->show();	

    debugBanner = new QLabel(this);
    debugBanner->setGeometry(QRect(QPoint(160,230),QSize(160,10)));
    debugBanner->hide();
    debugBanner->show();

    //topline text, date, clock, etc
    clock = new QPushButton(this);
    clock->setGeometry(QRect(QPoint(5,5),QSize(310,25)));
    clock->setFont(FONTSZ_CLOCK);
    clock->setObjectName("clock");
    clock->setText("January 1, 0000   00:00 PM" );   
    updateClock();

    //temperature
    currTemp = new QPushButton(this);
    currTemp->setGeometry(QRect(QPoint(250,210),QSize(65,25)));
    currTemp->setFont(FONTSZ_CLOCK);
    currTemp->setObjectName("temp");
    currTemp->setText("000 F" );   
    
    updateHAData();
   
    QTimer::singleShot(1000, this, SLOT(slotTimerPowerOn()));
}

void UI::updateClock()
{
    QDateTime now = QDateTime::currentDateTime();	
    clock->setText(now.toString("d MMMM yy     h:mm.ss AP"));
}

bool UI::updateHAData() {
    qDebug("Updating HA data from Vera...");
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    //QNetworkReply* reply = QNetworkAccessManager::get( manager->get( QNetworkRequest( QUrl("http://192.168.69.252:3480/data_request?id=sdata&output_format=xml"))));
    QUrl* vera_sdata = QUrl("http://192.168.69.252:3480/data_request?id=sdata&output_format=xml");
    //QNetworkRequest* req = QNetworkRequest(vera_sdata);
    //QNetworkReply* reply = manager->get(req);
    QNetworkReply* reply = manager->get( QNetworkRequest( QUrl("http://192.168.69.252:3480/data_request?id=sdata&output_format=xml")));

    qDebug << "waiting for reply for" << qPrintable(vera_sdata->toString());
    reply->waitForReadyRead(3000); //wait until doc is available
    qDebug() << "bytes received: " << reply->bytesAvailable();
    QXmlStreamReader xmlReader(reply);
    xmlReader.readNext();
    //Reading from the file
     while (!xmlReader.isEndDocument())
     {
         //qDebug("loop");
         if (xmlReader.isStartElement()) {
             QString name = xmlReader.name().toString();
             if (name == "device name") {
                 qDebug() << "dev: " << qPrintable(xmlReader.readElementText());
             }
         }
         else if (xmlReader.isEndElement())
         {
             xmlReader.readNext();
         }
     }
     if (xmlReader.hasError()) {
         qDebug("xmlReader crapped");
         return false;
     }

     return true;
}

void UI::slotErrorFatal(QString errorMsg)
{
    //screen->errorText->setText(errorMsg);
    //screen->ShowScreen(ScreenManager::ERROR_FATAL);
}

//main timer slot
void UI::slotTimerMain()
{
	updateClock();
}

//power-on timer slot, runs 5 sec after start-up to begin UI updates
void UI::slotTimerPowerOn()
{
    //screen->ShowScreen(ScreenManager::CLOCK);

    timerMain = new QTimer(this);
    connect(timerMain, SIGNAL(timeout()), this, SLOT(slotTimerMain()));
    timerMain->start(1000);
}
