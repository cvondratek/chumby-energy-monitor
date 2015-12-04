SOURCES += ../main.cpp \
           ../ui.cpp 
		
HEADERS += ../ui.h 

FORMS   +=

RESOURCES = ../project.qrc

DEFINES +=  HO=\\\"$$QMAKE_HOST.name\\\"

#QT += script

INCLUDEPATH += /home/cvondrachek/chumby_dev_2015/QtInstall/include
INCLUDEPATH += /home/cvondrachek/chumby_dev_2015/QtInstall/include/QtGui
INCLUDEPATH += /home/cvondrachek/chumby_dev_2015/QtInstall/include/QtCore

#override libs to get static linking
#LIBS += -Xlinker -Bstatic -lQtScript -lQtGui -lQtCore -lQtNetwork -Xlinker -Bdynamic

QMAKE_LIBDIR += /home/cvondrachek/chumby_dev_2015/QtInstall/lib

#QMAKE_LFLAGS += -Xlinker -Bstatic

#LIBS += -L../../QtLibs/qjson/qjson-66d10c/lib -lqjson

#add support for qjson
#INCLUDEPATH += ../../QtLibs/qjson/qjson-66d10c/include/
#LIBS += -L../../QtLibs/qjson/qjson-66d10c/lib -lqjson

# install
target.path = $$[QT_INSTALL_EXAMPLES]/tools/norse_B1
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS norse_B1.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/tools/norse_B1
INSTALLS += target sources
