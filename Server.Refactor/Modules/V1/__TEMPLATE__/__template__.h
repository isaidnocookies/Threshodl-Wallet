#ifndef __TEMPLATE___H
#define __TEMPLATE___H

#include "../../Interface/<<MODULE_NAME>>Interface/<<MODULE_NAME>>interface.h"

class App;
class <<MODULE_NAME>>ML;
class <<MODULE_NAME>> : public <<MODULE_NAME>>Interface
{
    friend class <<MODULE_NAME>>ML;
public:
    <<MODULE_NAME>>();
    virtual ~<<MODULE_NAME>>();
};

class <<MODULE_NAME>>ML
{
public:
    <<MODULE_NAME>>ML();
    static void * creator(void * pointerToAppObject);                                      // Returns a pointer to a new object
    static bool doInit(void * pointerToThis, void * pointerToAppObject);                   // Returns true on DoInit success
    static bool startInOwnThread();                                                        // Returns true if should be created and started in own thread
    static bool start(void * pointerToThis, void * pointerToAppObject);                    // Returns true on Start success
};

#endif // GRINDER_H
