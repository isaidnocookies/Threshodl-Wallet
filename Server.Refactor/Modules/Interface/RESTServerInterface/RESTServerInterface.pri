!contains( WALLETSERVERMODULES, RESTServerInterface ) {
    WALLETSERVERMODULES += RESTServerInterface

    HEADERS += \
        $$PWD/restserverinterface.h
}
