!contains( WALLETMODULES, WALLET_CORE ) {
    WALLETMODULES += WALLET_CORE

    !contains( QT, core ): QT += core
    !contains( QT, core ): QT += network
    !contains( QT, core ): QT += websockets

    linux | macos | ios {
        INCLUDEPATH += $$PWD/../libbtc/include
    }

    linux {
        !contains( LIBS, -lz ): LIBS += -lz
        !contains( LIBS, -lcrypto ): LIBS += -lcrypto
        !contains( LIBS, -lssl ): LIBS += -lssl
        !contains( LIBS, libbtc.a ):                    LIBS += $$PWD/../libbtc/.libs/libbtc.a
        !contains( LIBS, libsecp256k1.a ):              LIBS += $$PWD/../libbtc/src/secp256k1/.libs/libsecp256k1.a
    }

    macos {
        INCLUDEPATH += /usr/local/opt/openssl/include

        !contains( LIBS, /usr/local/opt/openssl/lib ):  LIBS += -L /usr/local/opt/openssl/lib
        !contains( LIBS, -lssl ):                       LIBS += -lssl
        !contains( LIBS, -lcrypto ):                    LIBS += -lcrypto
        !contains( LIBS, libbtc.a ):                    LIBS += $$PWD/../MacDeps/libbtc.a
        !contains( LIBS, libsecp256k1.a ):              LIBS += $$PWD/../MacDeps/libsecp256k1.a
    }

    ios {
        INCLUDEPATH += /usr/local/opt/openssl/include

        !contains( LIBS, libssl.a ):        LIBS += $$PWD/../iOSDeps/libssl.a
        !contains( LIBS, libcrypto.a ):     LIBS += $$PWD/../iOSDeps/libcrypto.a
        !contains( LIBS, libbtc.a ):        LIBS += $$PWD/../iOSDeps/libbtc.a
        !contains( LIBS, libsecp256k1.a ):  LIBS += $$PWD/../iOSDeps/libsecp256k1.a
    }

    INCLUDEPATH += $$PWD

    HEADERS += \
        $$PWD/core.h \
        $$PWD/sslbio.h \
        $$PWD/encryptionkey.h \
        $$PWD/certificate.h \
        $$PWD/digest.h \
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
        $$PWD/sslbio.cpp \
        $$PWD/encryptionkey.cpp \
        $$PWD/certificate.cpp \
        $$PWD/digest.cpp \
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
