!contains( WALLETSERVERMODULES, CertificateManagerInterface ) {
    WALLETSERVERMODULES += CertificateManagerInterface

    HEADERS += \
        $$PWD/certificatemanagerinterface.h
}
