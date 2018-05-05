!contains( WALLETSERVERMODULES, UNITTESTS ) {
    WALLETSERVERMODULES += UNITTESTS

    include($$PWD/DatabaseUT/DatabaseUT.pri)
}
