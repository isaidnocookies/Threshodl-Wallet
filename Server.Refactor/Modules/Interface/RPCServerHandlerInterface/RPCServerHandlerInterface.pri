!contains( WALLETSERVERMODULES, RPCServerHandlerInterface ) {
    WALLETSERVERMODULES += RPCServerHandlerInterface

    HEADERS += \
        $$PWD/rpcserverhandlerinterface.h
}
