#-------------------------------------------------
#
# Project created by QtCreator 2018-05-06T15:15:17
#
#-------------------------------------------------

QT       += core network
QT       -= gui

TARGET = JCSLP_Test
CONFIG += c++11 console
CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

HEADERS += \
        jcslpsa.h \
    jcslppdu.h \
    jcslpservice.h

SOURCES += \
	main.cpp \
        jcslpsa.cpp \
    jcslppdu.cpp \
    jcslpservice.cpp
