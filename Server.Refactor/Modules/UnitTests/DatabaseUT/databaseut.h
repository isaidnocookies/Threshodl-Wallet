#ifndef DATABASEUT_H
#define DATABASEUT_H

#include "../../V1/Database/database.h"

class DatabaseUT
{
public:
    DatabaseUT();
    static void * creator(void * pointerToAppObject);                                      // Returns a pointer to a new object
    static bool doInit(void * pointerToThis, void * pointerToAppObject);                   // Returns true on DoInit success
    static bool startInOwnThread();                                                        // Returns true if should be created and started in own thread
    static bool start(void * pointerToThis, void * pointerToAppObject);                    // Returns true on Start success
};

#endif  // DATABASEUT_H
