!contains( WALLETSERVERMODULES, Interface ) {
    WALLETSERVERMODULES += Interface

    include($$PWD/RecordsManagerInterface/RecordsManagerInterface.pri)
    include($$PWD/LogsManagerInterface/LogsManagerInterface.pri)
}
