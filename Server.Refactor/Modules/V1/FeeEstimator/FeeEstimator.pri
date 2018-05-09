!contains( WALLETSERVERMODULES, FeeEstimatorV1 ) {
    WALLETSERVERMODULES += FeeEstimatorV1

    HEADERS += \
        $$PWD/feeestimator.h

    SOURCES += \
        $$PWD/feeestimator.cpp
}
