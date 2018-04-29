!contains( WALLETSERVERMODULES, RecordsManagerInterface ) {
    WALLETSERVERMODULES += RecordsManagerInterface

    INCLUDEPATH=$$PWD

    HEADERS += \
        $$PWD/recordsmanagerinterface.h

    SOURCES += \
        $$PWD/recordsmanagerinterface.cpp
}
