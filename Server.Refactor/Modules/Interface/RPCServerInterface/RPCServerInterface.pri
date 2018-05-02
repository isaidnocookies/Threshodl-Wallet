!contains( WALLETSERVERMODULES, RPCServerInterface ) {
    WALLETSERVERMODULES += RPCServerInterface

    HEADERS += \
        $$PWD/rpcserverinterface.h
}
