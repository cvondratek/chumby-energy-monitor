#ifndef SCREENMANAGER_H
#define SCREENMANAGER_H

#include <QWidget>
#include <QFrame>
#include <QLabel>
#include <QMainWindow>
#include <QGraphicsView>
#include <QtGui/QPushButton>
#include <QtGui/QLineEdit>

class ScreenManager : public QMainWindow
{
    Q_OBJECT

public:
    ScreenManager(QWidget* parent);
    enum SCREENS {LOGO,CLOCK,ERROR_FATAL} ;
    void createElements();
    void ShowScreen(ScreenManager::SCREENS screen);
    QLabel* boldHeader;

public slots:

public:
    //TODO: wrap mods to these
    QLabel* debugBanner;
    QLabel* versionBanner;
    QLabel* errorText;
    QLabel* scrText;

    QPushButton clockA[6];
    QPushButton clockB[6];
    QPushButton clockColon1;
    QPushButton clockColon2;
    int clockState;

    void hlprUITheme(bool dark);

private:
    QString AppStyleSheet;
    QList<QWidget*> visibleObjects;

    QLabel* imgLogo;


    void makeElementVisible(QWidget* el);
    void hlprScreenErrorFatal();
    void hlprScreenLogo();
    void hlprScreenClock();


    void SetupClockDigit(QPushButton* c, int pos);

    QWidget* myParent;
};



#endif // SCREENMANAGER_H
