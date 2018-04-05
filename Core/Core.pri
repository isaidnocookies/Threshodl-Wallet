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
        $$PWD/rpcfield.h \
        $$PWD/rpcmessage.h \
    	$$PWD/rpcmessagepingrequest.h \
        $$PWD/rpcmessagepingreply.h \
        $$PWD/bitcoininterface.h \
        $$PWD/rawtransaction.h \
        $$PWD/entity.h \
        $$PWD/walletentity.h \
        $$PWD/billentity.h \
        $$PWD/sqliteinterface.h \
        $$PWD/utils.h

    SOURCES += \
        $$PWD/core.cpp \
        $$PWD/microwallet.cpp \
        $$PWD/encapsulatedmicrowallet.cpp \
        $$PWD/rpcmessage.cpp \
	$$PWD/rpcmessagepingrequest.cpp \
        $$PWD/rpcmessagepingreply.cpp \
        $$PWD/bitcoininterface.cpp \
        $$PWD/rawtransaction.cpp \
        $$PWD/entity.cpp \
        $$PWD/walletentity.cpp \
        $$PWD/billentity.cpp \
        $$PWD/sqliteinterface.cpp

    OTHER_FILES += \
        $$PWD/rpcmessages.h \
        $$PWD/rpcmessages.cpp
}

HEADERS += \
    $$PWD/rpcmessagecreateaccountrequest.h \
    $$PWD/rpcmessagecreateaccountreply.h

SOURCES += \
    $$PWD/rpcmessagecreateaccountrequest.cpp \
    $$PWD/rpcmessagecreateaccountreply.cpp
