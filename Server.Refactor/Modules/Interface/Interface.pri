!contains( WALLETSERVERMODULES, Interface ) {
    WALLETSERVERMODULES += Interface

    include($$PWD/RecordsManagerInterface/RecordsManagerInterface.pri)
}
