!contains( WALLETSERVERMODULES, MODULES ) {
    WALLETSERVERMODULES += MODULES

    include($$PWD/Interface/Interface.pri)
    include($$PWD/V1/V1.pri)
}
