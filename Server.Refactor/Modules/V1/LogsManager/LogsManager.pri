!contains( WALLETSERVERMODULES, LogsManagerV1 ) {
    WALLETSERVERMODULES += LogsManagerV1

    HEADERS += \
        $$PWD/logsmanager.h

    SOURCES += \
        $$PWD/logsmanager.cpp
}
