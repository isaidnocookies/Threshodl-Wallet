!contains( WALLETSERVERMODULES, DownloaderInterface ) {
    WALLETSERVERMODULES += DownloaderInterface

    HEADERS += \
        $$PWD/downloaderinterface.h
}
