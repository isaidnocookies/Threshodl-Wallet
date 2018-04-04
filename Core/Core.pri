!contains( WALLETMODULES, WALLET_CORE ) {
    WALLETMODULES += WALLET_CORE

    !contains( QT, core ): QT += core

    include($$PWD/../CertLib/CertLib.pri)
    include($$PWD/../RPC/RPC.pri)

    linux | macos {
        INCLUDEPATH += -I $$PWD/../libbtc/include
    }

    linux {
        LIBS += $$PWD/../libbtc/.libs/libbtc.a $$PWD/../libbtc/src/secp256k1/.libs/libsecp256k1.a
    }

    macos {
        LIBS += $$PWD/../MacDeps/libbtc.a $$PWD/../MacDeps/libsecp256k1.a
    }

    INCLUDEPATH += $$PWD

    HEADERS += \
        $$PWD/core.h \
        $$PWD/microwallet.h \
        $$PWD/encapsulatedmicrowallet.h \
        $$PWD/bill.h \
        $$PWD/wallet.h \
        $$PWD/rpcfield.h \
        $$PWD/rpcmessage.h \
    	$$PWD/rpcmessagepingrequest.h \
        $$PWD/rpcmessagepingreply.h \
        $$PWD/bitcoininterface.h \
        $$PWD/utils.h

    SOURCES += \
        $$PWD/core.cpp \
        $$PWD/microwallet.cpp \
        $$PWD/encapsulatedmicrowallet.cpp \
        $$PWD/bill.cpp \
        $$PWD/wallet.cpp \
        $$PWD/rpcmessage.cpp \
	$$PWD/rpcmessagepingrequest.cpp \
        $$PWD/rpcmessagepingreply.cpp \
        $$PWD/bitcoininterface.cpp

    OTHER_FILES += \
        $$PWD/rpcmessages.h \
        $$PWD/rpcmessages.cpp
}

HEADERS += \
    $$PWD/rawtransaction.h

SOURCES += \
    $$PWD/rawtransaction.cpp
