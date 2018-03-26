!contains( WALLETMODULES, WALLET_CERTLIB ) {
    WALLETMODULES += WALLET_CERTLIB

    !contains( QT, core ): QT += core

    linux {
        !contains( LIBS, -lz ): LIBS += -lz
        !contains( LIBS, -lcrypto ): LIBS += -lcrypto
        !contains( LIBS, -lssl ): LIBS += -lssl
    }

    macos {
        INCLUDEPATH += /usr/local/opt/openssl/include
        !contains( LIBS, /usr/local/opt/openssl/lib ): LIBS += -L /usr/local/opt/openssl/lib
        !contains( LIBS, -lssl ): LIBS += -lssl
        !contains( LIBS, -lcrypto ): LIBS += -lcrypto
    }

    INCLUDEPATH += $$PWD

    HEADERS += \
        $$PWD/sslbio.h \
        $$PWD/encryptionkey.h \
        $$PWD/certificate.h \
        $$PWD/digest.h

    SOURCES += \
        $$PWD/sslbio.cpp \
        $$PWD/encryptionkey.cpp \
        $$PWD/certificate.cpp \
        $$PWD/digest.cpp
}
