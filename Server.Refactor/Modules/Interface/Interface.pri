!contains( WALLETSERVERMODULES, Interface ) {
    WALLETSERVERMODULES += Interface

    include($$PWD/CertificateManagerInterface/CertificateManagerInterface.pri)
    include($$PWD/LogsManagerInterface/LogsManagerInterface.pri)
    include($$PWD/RecordsManagerInterface/RecordsManagerInterface.pri)
    include($$PWD/DatabaseInterface/DatabaseInterface.pri)
    include($$PWD/DownloaderInterface/DownloaderInterface.pri)
    include($$PWD/RESTServerInterface/RESTServerInterface.pri)
    include($$PWD/RESTHandlerInterface/RESTHandlerInterface.pri)
    include($$PWD/WCPServerHandlerInterface/WCPServerHandlerInterface.pri)
    include($$PWD/GrinderInterface/GrinderInterface.pri)
    include($$PWD/FeeEstimatorInterface/FeeEstimatorInterface.pri)
}
