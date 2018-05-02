!contains( WALLETSERVERMODULES, RecordsManagerV1 ) {
    WALLETSERVERMODULES += RecordsManagerV1

    HEADERS += \
        $$PWD/recordsmanager.h

    SOURCES += \
        $$PWD/recordsmanager.cpp
}
