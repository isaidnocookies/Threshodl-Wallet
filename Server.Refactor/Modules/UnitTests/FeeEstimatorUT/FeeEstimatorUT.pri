!contains( WALLETSERVERMODULES, FeeEstimatorUT ) {
    WALLETSERVERMODULES += FeeEstimatorUT

    HEADERS += \
        $$PWD/feeestimatorut.h

    SOURCES += \
        $$PWD/feeestimatorut.cpp
}
