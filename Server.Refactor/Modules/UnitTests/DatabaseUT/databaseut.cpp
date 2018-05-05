#include "databaseut.h"
#include "modulelinker.h"

#include <QDebug>
#include <QString>
#include <QStringList>

#ifdef UNIT_TEST_ON
static DatabaseUT   _gRegisterModuleLinker;
#else
static void *       _gRegisterModuleLinker = nullptr; // To prevent compiler errors
#endif

DatabaseUT::DatabaseUT()
{
    QStringList lDependencies = QStringList() << QStringLiteral("LogsManager-1") << QStringLiteral("CertificateManager-1") << QStringLiteral("Database-1");
    ModuleLinker::registerModuleWithDependencies(QStringLiteral("DatabaseUnitTest-1"),lDependencies,DatabaseUT::creator,DatabaseUT::doInit,DatabaseUT::start,DatabaseUT::startInOwnThread);
}

void *DatabaseUT::creator(void *pointerToAppObject)
{
    Q_UNUSED(pointerToAppObject)
    return &_gRegisterModuleLinker;
}

bool DatabaseUT::doInit(void *pointerToThis, void *pointerToAppObject)
{
    Q_UNUSED(pointerToThis)
    Q_UNUSED(pointerToAppObject)

    qDebug() << __func__;
    return true;
}

bool DatabaseUT::startInOwnThread()
{ return false; }

bool DatabaseUT::start(void *pointerToThis, void *pointerToAppObject)
{
    Q_UNUSED(pointerToThis)
    Q_UNUSED(pointerToAppObject)

    qDebug() << __func__;
    return true;
}
