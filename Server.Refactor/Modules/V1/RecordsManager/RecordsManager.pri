!contains( WALLETSERVERMODULES, RecordsManager ) {
    WALLETSERVERMODULES += RecordsManager

    INCLUDEPATH=$$PWD

    HEADERS += \
        $$PWD/recordsmanager.h

    SOURCES += \
        $$PWD/recordsmanager.cpp
}
