!contains( WALLETMODULES, WALLET_CORE ) {
    WALLETMODULES += WALLET_CORE

    !contains( QT, core ): QT += core
    !contains( QT, core ): QT += network
    !contains( QT, core ): QT += websockets

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

    include($$PWD/../CertLib/CertLib.pri)

    INCLUDEPATH += $$PWD

    HEADERS += \
        $$PWD/core.h \
        $$PWD/qstringmath.h \
        $$PWD/walletdatacore.h \
        $$PWD/wallet.h \
        $$PWD/bitcoinwallet.h \
        $$PWD/wallets.h \
        $$PWD/wcpfield.h \
        $$PWD/wcpmessage.h \
        $$PWD/wcpconnection.h \
        $$PWD/wcpserver.h \
        $$PWD/wcp.h \
        $$PWD/wcpmessages/wcpmessagecheckownershipofmicrowalletsreply.h \
        $$PWD/wcpmessages/wcpmessagecheckownershipofmicrowalletsrequest.h \
        $$PWD/wcpmessages/wcpmessagecompletemicrowalletsreply.h \
        $$PWD/wcpmessages/wcpmessagecompletemicrowalletsrequest.h \
        $$PWD/wcpmessages/wcpmessagecreateaccountreply.h \
        $$PWD/wcpmessages/wcpmessagecreateaccountrequest.h \
        $$PWD/wcpmessages/wcpmessagecreatemicrowalletpackagereply.h \
        $$PWD/wcpmessages/wcpmessagecreatemicrowalletpackagerequest.h \
        $$PWD/wcpmessages/wcpmessagepingreply.h \
        $$PWD/wcpmessages/wcpmessagepingrequest.h \
        $$PWD/wcpmessages/wcpmessagereassignmicrowalletsreply.h \
        $$PWD/wcpmessages/wcpmessagereassignmicrowalletsrequest.h \
        $$PWD/wcpmessages.h

    SOURCES += \
        $$PWD/qstringmath.cpp \
        $$PWD/walletdatacore.cpp \
        $$PWD/bitcoinwallet.cpp \
        $$PWD/wcpmessage.cpp \
        $$PWD/wcpconnection.cpp \
        $$PWD/wcpserver.cpp \
        $$PWD/wcpmessages/wcpmessagecheckownershipofmicrowalletsreply.cpp \
        $$PWD/wcpmessages/wcpmessagecheckownershipofmicrowalletsrequest.cpp \
        $$PWD/wcpmessages/wcpmessagecompletemicrowalletsreply.cpp \
        $$PWD/wcpmessages/wcpmessagecompletemicrowalletsrequest.cpp \
        $$PWD/wcpmessages/wcpmessagecreateaccountreply.cpp \
        $$PWD/wcpmessages/wcpmessagecreateaccountrequest.cpp \
        $$PWD/wcpmessages/wcpmessagecreatemicrowalletpackagereply.cpp \
        $$PWD/wcpmessages/wcpmessagecreatemicrowalletpackagerequest.cpp \
        $$PWD/wcpmessages/wcpmessagepingreply.cpp \
        $$PWD/wcpmessages/wcpmessagepingrequest.cpp \
        $$PWD/wcpmessages/wcpmessagereassignmicrowalletsrequest.cpp \
        $$PWD/wcpmessages/wcpmessagereassignmicrowalletsreply.cpp
}
