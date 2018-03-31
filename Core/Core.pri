!contains( WALLETMODULES, WALLET_CORE ) {
    WALLETMODULES += WALLET_CORE

    !contains( QT, core ): QT += core

    include($$PWD/../CertLib/CertLib.pri)
    include($$PWD/../RPC/RPC.pri)

    INCLUDEPATH += $$PWD

    HEADERS += \
        $$PWD/core.h \
        $$PWD/microwallet.h \
        $$PWD/encapsulatedmicrowallet.h

    SOURCES += \
        $$PWD/core.cpp \
        $$PWD/microwallet.cpp \
        $$PWD/encapsulatedmicrowallet.cpp
}

HEADERS += \
    $$PWD/rpcmessages.h

SOURCES += \
    $$PWD/rpcmessages.cpp
