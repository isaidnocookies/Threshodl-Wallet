!contains( WALLETSERVERMODULES, RPCHandlerInterface ) {
    WALLETSERVERMODULES += RPCHandlerInterface

    HEADERS += \
        $$PWD/rpchandlerinterface.h
}
