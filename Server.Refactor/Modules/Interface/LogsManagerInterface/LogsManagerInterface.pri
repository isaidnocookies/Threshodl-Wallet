!contains( WALLETSERVERMODULES, LogsManagerInterface ) {
    WALLETSERVERMODULES += LogsManagerInterface

    HEADERS += \
        $$PWD/logsmanagerinterface.h
}
