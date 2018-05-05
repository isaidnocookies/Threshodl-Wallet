!contains( WALLETSERVERMODULES, MODULES ) {
    WALLETSERVERMODULES += MODULES

    include(QtWebApp/httpserver/httpserver.pri)
    include($$PWD/Interface/Interface.pri)
    include($$PWD/V1/V1.pri)

    contains(DEFINES,UNIT_TEST_ON) {
        include($$PWD/UnitTests/UnitTests.pri)
    }
}
