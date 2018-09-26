QT += quick core network gui
CONFIG += c++11 qzxing_qml qzxing_multimedia

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

QML_IMPORT_PATH += /Users/christopherforte/Qt/5.10.1/Src/qtgraphicaleffects

ios {
    QMAKE_INFO_PLIST = ios/Info.plist

    ios_icon.files = $$files($$PWD/ios/AppIcon.appiconset/Icon-App*.png)

    QMAKE_BUNDLE_DATA += ios_icon
    QMAKE_CFLAGS += -gdwarf-2
    QMAKE_CXXFLAGS += -gdwarf-2
}

include (../Core/Core.pri)
include (QzXing/QZXing.pri)

HEADERS += \
        useraccount.h \
        platforminformation.h \
        createusername.h \
        walletaccount.h \
        threshodltools.h \
        myqsettingsmanager.h \
        downloadworker.h \
        SmtpClient/emailaddress.h \
        SmtpClient/mimeattachment.h \
        SmtpClient/mimecontentformatter.h \
        SmtpClient/mimefile.h \
        SmtpClient/mimehtml.h \
        SmtpClient/mimeinlinefile.h \
        SmtpClient/mimemessage.h \
        SmtpClient/mimemultipart.h \
        SmtpClient/mimepart.h \
        SmtpClient/mimetext.h \
        SmtpClient/quotedprintable.h \
        SmtpClient/smtpclient.h \
        SmtpClient/smtpexports.h \
        SmtpClient/SmtpMime \
        darkwallettools.h

SOURCES += \
        main.cpp \
        useraccount.cpp \
        platforminformation.cpp \
        createusername.cpp \
        walletaccount.cpp \
        threshodltools.cpp \
        myqsettingsmanager.cpp \
        downloadworker.cpp \
        SmtpClient/emailaddress.cpp \
        SmtpClient/mimeattachment.cpp \
        SmtpClient/mimecontentformatter.cpp \
        SmtpClient/mimefile.cpp \
        SmtpClient/mimehtml.cpp \
        SmtpClient/mimeinlinefile.cpp \
        SmtpClient/mimemessage.cpp \
        SmtpClient/mimemultipart.cpp \
        SmtpClient/mimepart.cpp \
        SmtpClient/mimetext.cpp \
        SmtpClient/quotedprintable.cpp \
        SmtpClient/smtpclient.cpp \
        darkwallettools.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=
