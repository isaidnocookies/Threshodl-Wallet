!contains( WALLETMODULES, WALLET_CORE ) {
    WALLETMODULES += WALLET_CORE

    !contains( QT, core ): QT += core

    include($$PWD/../CertLib/CertLib.pri)
    include($$PWD/../RPC/RPC.pri)

    linux | macos | ios {
        INCLUDEPATH += $$PWD/../libbtc/include
    }

    linux {
        LIBS += $$PWD/../libbtc/.libs/libbtc.a $$PWD/../libbtc/src/secp256k1/.libs/libsecp256k1.a
    }

    macos {
        LIBS += $$PWD/../MacDeps/libbtc.a $$PWD/../MacDeps/libsecp256k1.a
    }

    ios {
        LIBS += $$PWD/../iOSDeps/libbtc.a $$PWD/../iOSDeps/libsecp256k1.a
    }

    INCLUDEPATH += $$PWD

    HEADERS += \
        $$PWD/core.h \
        $$PWD/wallet.h \
        $$PWD/bitcoinwallet.h \
        $$PWD/rpcfield.h \
        $$PWD/rpcmessage.h \
    	$$PWD/rpcmessagepingrequest.h \
        $$PWD/rpcmessagepingreply.h \
        $$PWD/rpcmessagecreateaccountrequest.h \
        $$PWD/rpcmessagecreateaccountreply.h \
        $$PWD/bitcoininterface.h \
        $$PWD/walletdatacore.h

    SOURCES += \
        $$PWD/core.cpp \
        $$PWD/bitcoinwallet.cpp \
        $$PWD/rpcmessage.cpp \
	$$PWD/rpcmessagepingrequest.cpp \
        $$PWD/rpcmessagepingreply.cpp \
        $$PWD/rpcmessagecreateaccountrequest.cpp \
        $$PWD/rpcmessagecreateaccountreply.cpp \
        $$PWD/bitcoininterface.cpp

    OTHER_FILES += \
        $$PWD/rawtransaction.cpp \
        $$PWD/entity.cpp \
        $$PWD/walletentity.cpp \
        $$PWD/billentity.cpp \
        $$PWD/rawtransaction.h \
        $$PWD/entity.h \
        $$PWD/walletentity.h \
        $$PWD/billentity.h \
        $$PWD/utils.h \
        $$PWD/sqliteinterface.cpp \
        $$PWD/sqliteinterface.h \
        $$PWD/rpcmessages.h \
        $$PWD/rpcmessages.cpp \
        $$PWD/walletdatacore.cpp
}

HEADERS += \

SOURCES += \
