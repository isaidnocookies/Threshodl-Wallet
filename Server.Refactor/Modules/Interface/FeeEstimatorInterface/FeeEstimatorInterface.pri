!contains( WALLETSERVERMODULES, FeeEstimatorInterface ) {
    WALLETSERVERMODULES += FeeEstimatorInterface

    HEADERS += \
        $$PWD/feeestimatorinterface.h
}
