!contains( WALLETSERVERMODULES, DatabaseUT ) {
    WALLETSERVERMODULES += DatabaseUT

    HEADERS += \
        $$PWD/databaseut.h

    SOURCES += \
        $$PWD/databaseut.cpp
}
