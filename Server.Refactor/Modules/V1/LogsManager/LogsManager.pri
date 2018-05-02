!contains( WALLETSERVERMODULES, LogsManager ) {
    WALLETSERVERMODULES += LogsManager

    HEADERS += \
        $$PWD/logsmanager.h

    SOURCES += \
        $$PWD/logsmanager.cpp
}
