#-------------------------------------------------
#
# Project created by QtCreator 2018-04-04T08:45:09
#
#-------------------------------------------------

QT       += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11 console

TARGET = ThresHoDLApp
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

DEFINES += ENABLE_RPCSERVER

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

#include (../Core/Core.pri)
include (../QrCodeModule/QrCodeModule.pri)

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    createaccount.cpp \
    useraccount.cpp \
    brightwallet.cpp \
    darkwallet.cpp \
    brightsendview.cpp \
    sendtodarkview.cpp \
    darksendview.cpp \
    sendtobrightview.cpp

HEADERS += \
    mainwindow.h \
    createaccount.h \
    useraccount.h \
    brightwallet.h \
    darkwallet.h \
    globalsandstyle.h \
    createaccount.h \
    brightsendview.h \
    sendtodarkview.h \
    darksendview.h \
    sendtobrightview.h

FORMS += \
    mainwindow.ui \
    createaccount.ui \
    brightwallet.ui \
    darkwallet.ui \
    brightsendview.ui \
    sendtodarkview.ui \
    darksendview.ui \
    sendtobrightview.ui
