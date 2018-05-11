!contains( WALLETSERVERMODULES, WCPServerHandlerInterface ) {
    WALLETSERVERMODULES += WCPServerHandlerInterface

    HEADERS += \
        $$PWD/wcpserverhandlerinterface.h

    SOURCES += \
        $$PWD/wcpserverhandlerinterface.cpp
}
