!contains( WALLETMODULES, WALLET_CORE ) {
    WALLETMODULES += WALLET_CORE

    !contains( QT, core ):          QT += core
    !contains( QT, network ):       QT += network
#    !contains( QT, websockets ):    QT += websockets

    INCLUDEPATH += $$PWD

    HEADERS += \
        $$PWD/core.h \
        $$PWD/qstringmath.h \
        $$PWD/walletdatacore.h \
        $$PWD/wallet.h

    SOURCES += \
        $$PWD/qstringmath.cpp \
        $$PWD/walletdatacore.cpp
}
