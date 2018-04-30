!contains( WALLETSERVERMODULES, RecordsManager ) {
    WALLETSERVERMODULES += RecordsManager

    HEADERS += \
        $$PWD/recordsmanager.h

    SOURCES += \
        $$PWD/recordsmanager.cpp
}
