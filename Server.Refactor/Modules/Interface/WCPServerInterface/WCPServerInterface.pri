!contains( WALLETSERVERMODULES, WCPServerInterface ) {
    WALLETSERVERMODULES += WCPServerInterface

    DEFINES += ENABLE_WCPSERVER

    HEADERS += \
        $$PWD/wcpserverinterface.h
}
