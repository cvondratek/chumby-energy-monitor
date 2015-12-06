/*f
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
#define FONTSZ_HOUSEINFO QFont("Open Sans", 16)

UI::UI() : QMainWindow()
{

    networkAccessActive=false;

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

    //outside temperature
/*    currTemp = new QPushButton(this);
    currTemp->setGeometry(QRect(QPoint(250,100),QSize(65,25)));
    currTemp->setFont(FONTSZ_LARGE);
    currTemp->setObjectName("temp");
    currTemp->setText("000 F" );   
*/
    //topline text, date, clock, etc
    houseInfo = new QPushButton(this);
    houseInfo->setGeometry(QRect(QPoint(5,200),QSize(310,35)));
    houseInfo->setFont(FONTSZ_HOUSEINFO);
    houseInfo->setObjectName("houseInfo");
    houseInfo->setText("Up:00F Down:00F XXXX Watts" );

    updateHAData();


    timerHAUpdate = new QTimer(this);
    connect(timerHAUpdate, SIGNAL(timeout()), this, SLOT(updateHAData()));
    timerHAUpdate->start(60000);

    QTimer::singleShot(1000, this, SLOT(slotTimerPowerOn()));
}

void UI::updateClock()
{
    QDateTime now = QDateTime::currentDateTime();	
    clock->setText(now.toString("d MMMM yy     h:mm.ss AP"));
}

void UI::parseHAData(QNetworkReply* reply)
{
    int watts=0;
    int tempUpstairs=0;
    int tempDownstairs=0;
    int tempOutside=0;

    qDebug() << "HA: bytes received: " << reply->bytesAvailable();
    //qDebug() << reply->readAll();

    QXmlStreamReader xmlReader(reply);
    while(!xmlReader.atEnd())
    {
        xmlReader.readNext();
        if(xmlReader.isStartElement())
        {
            //qDebug() << xmlReader.readElementText(QXmlStreamReader::IncludeChildElements);
            QString sec(xmlReader.name().toString());
            //qDebug() << sec;
            if(sec == "device")
            {
                QString deviceName = "noDevice";

                QXmlStreamAttributes attribs = xmlReader.attributes();

                if(attribs.hasAttribute("name"))
                {
                    deviceName = attribs.value("name").toString();
                    //qDebug() << "\tname=" << attribs.value("name").toString();
                }

                if(deviceName=="Home Energy Monitor")
                {
                    if(attribs.hasAttribute("watts"))
                    {
                        watts = attribs.value("watts").toString().toInt();
                        qDebug() << "HA: Watts = " << watts;
                    }
                    else
                        qDebug("HA: Error - No \"watts\" attribute for Home Energy Monitor!");
                }

                if(deviceName=="Downstairs")
                {
                    if(attribs.hasAttribute("temperature"))
                    {
                        tempDownstairs = attribs.value("temperature").toString().toInt();
                        qDebug() << "HA: Downstairs = " << tempDownstairs << " F";
                    }
                    else
                        qDebug("HA: Error - No \"temperature\" attribute for Downstairs!");
                }

                if(deviceName=="Upstairs")
                {
                    if(attribs.hasAttribute("temperature"))
                    {
                        tempUpstairs = attribs.value("temperature").toString().toInt();
                        qDebug() << "HA: Upstairs = " << tempUpstairs << " F";
                    }
                    else
                        qDebug("HA: Error - No \"temperature\" attribute for Upstairs!");
                }

                //update display
                QString dispStr;
                dispStr.sprintf("Up: %dF   Down: %dF     %d Watts", tempUpstairs,tempDownstairs,watts);
                houseInfo->setText(dispStr);

/*                if(attribs.hasAttribute("id"))
                    qDebug() << "\tid=" << attribs.value("id").toString();
                if(attribs.hasAttribute("status"))
                    qDebug() << "\tstatus=" << attribs.value("status").toString();
                if(attribs.hasAttribute("level"))
                    qDebug() << "\tlevel=" << attribs.value("level").toString();
*/



            }
        }
        else if(xmlReader.hasError())
            qDebug() << xmlReader.errorString();

    }

    reply->deleteLater();
    manager->deleteLater();
    networkAccessActive=false;

//   this->deleteLater();
//    exit(0);
}

void UI::updateHAData() {
    if(networkAccessActive)
    {
        qDebug("Previous network access hung!");
        return;
    }

    networkAccessActive=true;
    qDebug("Updating HA data from Vera...");
    manager = new QNetworkAccessManager(this);
    connect(manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(parseHAData(QNetworkReply*)));
    manager->get(QNetworkRequest(QUrl("http://192.168.69.252:3480/data_request?id=sdata&output_format=xml")));
    return;
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
