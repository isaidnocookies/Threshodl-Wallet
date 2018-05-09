#ifndef FEEESTIMATOR_H
#define FEEESTIMATOR_H

#include "../../Interface/FeeEstimatorInterface/feeestimatorinterface.h"

class App;
class FeeEstimatorML;
class FeeEstimator : public FeeEstimatorInterface
{
    friend class FeeEstimatorML;
protected:
    App *       mApp = nullptr;

public:
    FeeEstimator(QObject *iParent = nullptr);
    virtual ~FeeEstimator();

public slots:
    void downloaded( const QUrl iUrl, const QByteArray iData );

signals:
    void recordFee(const QString iCryptoCurrency, const QString iType, const QString iFee);
};

class FeeEstimatorML
{
public:
    FeeEstimatorML();
    static void * creator(void * pointerToAppObject);                                      // Returns a pointer to a new object
    static bool doInit(void * pointerToThis, void * pointerToAppObject);                   // Returns true on DoInit success
    static bool startInOwnThread();                                                        // Returns true if should be created and started in own thread
    static bool start(void * pointerToThis, void * pointerToAppObject);                    // Returns true on Start success
};

#endif // FEEESTIMATOR_H
