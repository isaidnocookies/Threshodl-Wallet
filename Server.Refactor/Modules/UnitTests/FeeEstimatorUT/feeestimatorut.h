#ifndef FEEESTIMATORUT_H
#define FEEESTIMATORUT_H

class FeeEstimatorUT
{
public:
    FeeEstimatorUT();
    static void * creator(void * pointerToAppObject);                                      // Returns a pointer to a new object
    static bool doInit(void * pointerToThis, void * pointerToAppObject);                   // Returns true on DoInit success
    static bool startInOwnThread();                                                        // Returns true if should be created and started in own thread
    static bool start(void * pointerToThis, void * pointerToAppObject);                    // Returns true on Start success
};

#endif // FEEESTIMATORUT_H
