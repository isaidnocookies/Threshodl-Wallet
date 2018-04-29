!contains( WALLETSERVERMODULES, V1 ) {
    WALLETSERVERMODULES += V1

    include($$PWD/RecordsManager/RecordsManager.pri)
}
