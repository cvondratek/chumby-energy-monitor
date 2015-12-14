/*
*  ui.h - Main UI View 
*  c. ProtoLogic, 2015, chris@protologicnw.com
*
*  Target: Qt Embedded 4.8x under Linux
*
*  Debug under Mac/PC is possible but not all input functions will work.
*
*  License: LGPL (dynamic link to QtEmb libraries)
*
*/
#ifndef UI_H
#define UI_H

#include <QWidget>
#include <QFrame>
#include <QMainWindow>
#include <QGraphicsView>
#include <QtGui/QPushButton>
#include <QtGui/QLineEdit>
#include <QKeyEvent>
#include <QThread>
#include <QtNetwork/QNetworkReply>

QT_BEGIN_NAMESPACE
class QLabel;
class QWidget;
class QPaintEvent;
class QPropertyAnimation;
class QLineEdit;
class ScreenManager;
QT_END_NAMESPACE

//! [0]
class UI : public QMainWindow
{
    Q_OBJECT

public slots:
    void slotErrorFatal(QString errorMsg);

public:
    UI();

public slots:
    void slotTimerMain();
    void slotTimerPowerOn();
    void parseHAData(QNetworkReply* reply);
    void updateHAData();

private:
    QTimer* timerMain;
    QTimer* timerHAUpdate;
    QNetworkAccessManager *manager;
    bool networkAccessActive;
    QLabel* boldHeader;
    QLabel* debugBanner;
    QLabel* versionBanner;
    QLabel* errorText;
    QLabel* scrText;
    QPushButton* clock;
    QPushButton* currTemp;
    QPushButton* houseInfo;
    QLabel* graphArea;
    QImage* tempGraph;

    QString AppStyleSheet;



    void updateClock();
};
//! [0]

#endif

