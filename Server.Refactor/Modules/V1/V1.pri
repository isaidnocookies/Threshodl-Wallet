!contains( WALLETSERVERMODULES, V1 ) {
    WALLETSERVERMODULES += V1

    include($$PWD/LogsManager/LogsManager.pri)
    include($$PWD/CertificateManager/CertificateManager.pri)
    include($$PWD/RecordsManager/RecordsManager.pri)
    include($$PWD/Downloader/Downloader.pri)
    include($$PWD/Database/Database.pri)
}
