!contains( WALLETSERVERMODULES, DownloaderV1 ) {
    WALLETSERVERMODULES += DownloaderV1

    HEADERS += \
        $$PWD/downloader.h

    SOURCES += \
        $$PWD/downloader.cpp
}
