QT += core network sql
QT -= gui

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

DEFINES += ENABLE_RPCSERVER

include(QtWebApp/logging/logging.pri)
include(QtWebApp/httpserver/httpserver.pri)
include($$PWD/../CertLib/CertLib.pri)
include($$PWD/../RPC/RPC.pri)

linux | macos {
    INCLUDEPATH += -I $$PWD/../libbtc/include
}

linux {
    LIBS += $$PWD/../libbtc/.libs/libbtc.a $$PWD/../libbtc/src/secp256k1/.libs/libsecp256k1.a
}

macos {
    LIBS += $$PWD/../MacDeps/libbtc.a $$PWD/../MacDeps/libsecp256k1.a
}

HEADERS += \
    main.h \
    downloader.h \
    recordsmanager.h \
    logsmanager.h \
    app.h \
    resthandler.h \
    restalphaexchangerate.h \
    restalphahandler.h \
    dbinterface.h \
    dbinterfacealpha.h \
    bitcoininterface.h \
    rpcserverhandler.h


SOURCES += \
    main.cpp \
    downloader.cpp \
    recordsmanager.cpp \
    logsmanager.cpp \
    app.cpp \
    resthandler.cpp \
    restalphaexchangerate.cpp \
    restalphahandler.cpp \
    dbinterfacealpha.cpp \
    bitcoininterface.cpp \
    rpcserverhandler.cpp

OTHER_FILES += \
    restalphamodeatomodeb.h \
    restalphamodebtomodeb.h \
    restalphamodebtomodea.h \
    restalphamanageaddress.h \
    restalphamodeatomodeb.cpp \
    restalphamodebtomodeb.cpp \
    restalphamodebtomodea.cpp \
    restalphamanageaddress.cpp

