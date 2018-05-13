!contains( WALLETSERVERMODULES, WCPV1 ) {
    WALLETSERVERMODULES += WCPV1

    HEADERS += \
        $$PWD/wcpserverhandler.h \
        $$PWD/wcpclient.h

    SOURCES += \
        $$PWD/wcpserverhandler.cpp \
        $$PWD/wcpclient.cpp
}
