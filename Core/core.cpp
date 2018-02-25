#include "core.h"

#include <QDebug>
#include <QMutex>
#include <QDir>
#include <QStandardPaths>
#include <QCoreApplication>

static QMutex               _sSharedInstanceLock;
static QSharedPointer<Core> _sSharedInstance;

Core::Core(const QString iStoragePath)
    : mStoragePath(iStoragePath)
{
    QDir lStoragePathDir(mStoragePath);
    if( ! lStoragePathDir.mkpath(mStoragePath) ) {
        qWarning() << "Storage Path does not exist or is not writable.";
    }
}

QSharedPointer<Core> Core::create(QSettings *iSettings)
{
    QMutexLocker            lLock{&_sSharedInstanceLock};
    QString                 lStoragePath = QStringLiteral("");
    QSharedPointer<Core>    lResult;

    if( ! _sSharedInstance ) {
        if(iSettings) {
            lStoragePath = iSettings->value(QStringLiteral("StoragePath"),lStoragePath).toString();
        }

        if( lStoragePath.isEmpty() ) {
#if defined(Q_OS_IOS)
#elif defined(Q_OS_ANDROID)
#elif defined(Q_OS_WIN32) || defined(Q_OS_MACOS) || defined(Q_OS_UNIX) || defined(Q_OS_LINUX)
            lStoragePath = QStringLiteral("%1%2%3")
                    .arg(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation))
                    .arg(QDir::separator())
                    .arg(qApp->applicationName());
#endif
        }

        lResult = QSharedPointer<Core>(new Core(lStoragePath));
        _sSharedInstance = lResult;
    }

    return lResult;
}

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
