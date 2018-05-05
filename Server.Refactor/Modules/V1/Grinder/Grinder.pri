!contains( WALLETSERVERMODULES, GrinderV1 ) {
    WALLETSERVERMODULES += GrinderV1

    HEADERS += \
        $$PWD/grinder.h

    SOURCES += \
        $$PWD/grinder.cpp
}
