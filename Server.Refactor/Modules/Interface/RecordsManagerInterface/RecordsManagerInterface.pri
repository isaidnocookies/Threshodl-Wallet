!contains( WALLETSERVERMODULES, RecordsManagerInterface ) {
    WALLETSERVERMODULES += RecordsManagerInterface

    HEADERS += \
        $$PWD/recordsmanagerinterface.h

    SOURCES += \
        $$PWD/recordsmanagerinterface.cpp
}
