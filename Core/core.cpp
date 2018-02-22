#include "core.h"
#include <QMutex>

static QMutex               _sSharedInstanceLock;
static QSharedPointer<Core> _sSharedInstance;

QSharedPointer<Core> Core::sharedInstance()
{
    QMutexLocker l{&_sSharedInstanceLock};
    return _sSharedInstance;
}

void Core::setSharedInstance(QSharedPointer<Core> iCoreRef)
{
    QMutexLocker l{&_sSharedInstanceLock};
    _sSharedInstance = iCoreRef;
}

Core::Core()
{
}
