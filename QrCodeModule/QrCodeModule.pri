!contains( WALLETMODULES, QRCODE_MODULES ) {
    WALLETMODULES += QRCODE_MODULES

    !contains( QT, core ):          QT += core
    !contains( QT, widgets ):       QT += widgets

    INCLUDEPATH += $$PWD

    SOURCES += \
        $$PWD/Nayuki/BitBuffer.cpp \
        $$PWD/Nayuki/QrCode.cpp \
        $$PWD/Nayuki/QrSegment.cpp \
        $$PWD/qrencoder.cpp

    HEADERS += \
        $$PWD/Nayuki/BitBuffer.hpp \
        $$PWD/Nayuki/QrCode.hpp \
        $$PWD/Nayuki/QrSegment.hpp \
        $$PWD/qrencoder.h
}
