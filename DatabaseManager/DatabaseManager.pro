QT -= gui
QT += sql

CONFIG += c++11 console
CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

#include($$PWD/../Core/Core.pri)

SOURCES += \
        main.cpp \
    walletentity.cpp \
    billentity.cpp \
    entity.cpp \
    qsettingsinterface.cpp \
    sqliteinterface.cpp

HEADERS += \
    walletentity.h \
    billentity.h \
    entity.h \
    utils.h \
    qsettingsinterface.h \
    sqliteinterface.h