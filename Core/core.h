#ifndef CORE_H
#define CORE_H

#include "core_global.h"

#include <QSharedPointer>

class CORESHARED_EXPORT Core
{

public:
    static QSharedPointer<Core> sharedInstance();
    static void setSharedInstance(QSharedPointer<Core> iCoreRef);

    Core();
};

#endif // CORE_H
