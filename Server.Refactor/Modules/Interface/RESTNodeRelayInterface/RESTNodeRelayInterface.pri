!contains( WALLETSERVERMODULES, RESTNodeRelayInterfaceInterface ) {
    WALLETSERVERMODULES += RESTNodeRelayInterfaceInterface

    HEADERS += \
        $$PWD/restnoderelayinterface.h
}
