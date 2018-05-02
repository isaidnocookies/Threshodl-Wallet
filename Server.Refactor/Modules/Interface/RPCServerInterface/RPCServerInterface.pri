!contains( WALLETSERVERMODULES, RPCServerInterface ) {
    WALLETSERVERMODULES += RPCServerInterface

    DEFINES += ENABLE_RPCSERVER

    HEADERS += \
        $$PWD/rpcserverinterface.h
}
