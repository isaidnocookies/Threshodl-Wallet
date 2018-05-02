!contains( WALLETSERVERMODULES, DatabaseInterface ) {
    WALLETSERVERMODULES += DatabaseInterface

    HEADERS += \
        $$PWD/databaseinterface.h
}
