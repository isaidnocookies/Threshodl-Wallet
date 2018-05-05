!contains( WALLETSERVERMODULES, GrinderInterface ) {
    WALLETSERVERMODULES += GrinderInterface

    HEADERS += \
        $$PWD/grinderinterface.h
}
