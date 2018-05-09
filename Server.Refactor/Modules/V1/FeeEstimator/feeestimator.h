#ifndef FEEESTIMATOR_H
#define FEEESTIMATOR_H

#include "../../Interface/FeeEstimatorInterface/FeeEstimatorinterface.h"

class App;
class FeeEstimatorML;
class FeeEstimator : public FeeEstimatorInterface
{
    friend class FeeEstimatorML;
public:
    FeeEstimator();
    virtual ~FeeEstimator();
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
