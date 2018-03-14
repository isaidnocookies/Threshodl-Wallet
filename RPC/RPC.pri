!contains( WALLETMODULES, WALLET_RPC ) {
    WALLETMODULES += WALLET_RPC

    !contains( QT, network ):       QT += network
    !contains( QT, websockets ):    QT += websockets

    INCLUDEPATH += $$PWD

    HEADERS += \
        $$PWD/rpc.h \
        $$PWD/rpcserver.h \
        $$PWD/rpcconnection.h

    SOURCES += \
        $$PWD/rpc.cpp \
        $$PWD/rpcserver.cpp \
        $$PWD/rpcconnection.cpp
}
