#ifndef CORE_H
#define CORE_H

#include "core_global.h"

#include <QSharedPointer>
#include <QSettings>

class CORESHARED_EXPORT Core
{
private:
    QString         mStoragePath;

    Core(const QString iStoragePath);

public:
    static QSharedPointer<Core> create(QSettings * iSettings);

    static QSharedPointer<Core> sharedInstance();
    static void setSharedInstance(QSharedPointer<Core> iCoreRef);
};

#endif // CORE_H
