!contains( WALLETSERVERMODULES_INTERFACE, RecordsManager ) {
    WALLETSERVERMODULES_INTERFACE += RecordsManager

    INCLUDEPATH=$$PWD

    HEADERS += \
        $$PWD/recordsmanagerinterface.h
}
