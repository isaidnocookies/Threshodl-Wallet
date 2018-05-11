!contains( WALLETSERVERMODULES, WCPInterface ) {
    WALLETSERVERMODULES += WCPInterface

    HEADERS += \
        $$PWD/wcpserverhandlerinterface.h \
        $$PWD/wcpclientinterface.h

    SOURCES += \
        $$PWD/wcpserverhandlerinterface.cpp \
        $$PWD/wcpclientinterface.cpp
}
