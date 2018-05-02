!contains( WALLETSERVERMODULES, Interface ) {
    WALLETSERVERMODULES += Interface

    include($$PWD/RecordsManagerInterface/RecordsManagerInterface.pri)
    include($$PWD/LogsManagerInterface/LogsManagerInterface.pri)
    include($$PWD/CertificateManagerInterface/CertificateManagerInterface.pri)
    include($$PWD/DatabaseInterface/DatabaseInterface.pri)
    include($$PWD/DownloaderInterface/DownloaderInterface.pri)
    include($$PWD/RESTHandlerInterface/RESTHandlerInterface.pri)
    include($$PWD/RPCServerInterface/RPCServerInterface.pri)
    include($$PWD/RPCServerHandlerInterface/RPCServerHandlerInterface.pri)
}
