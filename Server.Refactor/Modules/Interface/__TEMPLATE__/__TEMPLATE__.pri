!contains( WALLETSERVERMODULES, <<MODULE_NAME>>Interface ) {
    WALLETSERVERMODULES += <<MODULE_NAME>>Interface

    HEADERS += \
        $$PWD/<<HEADER_FILE>>.h
}
