!contains( WALLETSERVERMODULES, CertificateManagerV1 ) {
    WALLETSERVERMODULES += CertificateManager

    HEADERS += \
        $$PWD/certificatemanager.h

    SOURCES += \
        $$PWD/certificatemanager.cpp
}
