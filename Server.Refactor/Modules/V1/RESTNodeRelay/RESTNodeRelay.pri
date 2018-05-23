!contains( WALLETSERVERMODULES, RESTNodeRelayV1 ) {
    WALLETSERVERMODULES += RESTNodeRelayV1

    HEADERS += \
        $$PWD/restnoderelay.h

    SOURCES += \
        $$PWD/restnoderelay.cpp
}
