!contains( WALLETSERVERMODULES, RESTHandlerInterface ) {
    WALLETSERVERMODULES += RESTHandlerInterface

    HEADERS += \
        $$PWD/resthandlerinterface.h
}
