!contains( WALLETSERVERMODULES, GrinderUT ) {
    WALLETSERVERMODULES += GrinderUT

    HEADERS += \
        $$PWD/grinderut.h

    SOURCES += \
        $$PWD/grinderut.cpp
}
