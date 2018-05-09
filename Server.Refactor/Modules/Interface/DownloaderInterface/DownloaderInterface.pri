!contains( WALLETSERVERMODULES, DownloaderInterface ) {
    WALLETSERVERMODULES += DownloaderInterface

    HEADERS += \
        $$PWD/downloaderinterface.h

    SOURCES += \
        $$PWD/downloaderinterface.cpp
}

