SOURCES += main.cpp \
           ui.cpp	

HEADERS += ui.h 

FORMS   +=

DEFINES +=  HO=\\\"$$QMAKE_HOST.name\\\"

RESOURCES = project.qrc

QT += script

LIBS += -lQtNetwork

# install
target.path = $$[QT_INSTALL_EXAMPLES]/tools/norse_preA
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS norse_preA.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/tools/norse_preA
INSTALLS += target sources

OTHER_FILES += \
    MainApp.qss
