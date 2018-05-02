!contains( WALLETSERVERMODULES, MODULES ) {
    WALLETSERVERMODULES += MODULES

    include(QtWebApp/httpserver/httpserver.pri)
    include($$PWD/Interface/Interface.pri)
    include($$PWD/V1/V1.pri)
}
