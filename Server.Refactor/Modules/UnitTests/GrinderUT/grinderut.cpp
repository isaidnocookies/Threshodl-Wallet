#include "grinderut.h"
#include "modulelinker.h"
#include "app.h"
#include "../../V1/Grinder/grinder.h"

#include <QDebug>
#include <QString>
#include <QStringList>

#ifdef UNIT_TEST_ON
static GrinderUT    _gRegisterModuleLinker;
#else
static void *       _gRegisterModuleLinker = nullptr; // To prevent compiler errors
#endif

GrinderUT::GrinderUT()
{
    QStringList lDependencies = QStringList() << QStringLiteral("LogsManager-1") << QStringLiteral("Grinder-1");
    ModuleLinker::registerModuleWithDependencies(QStringLiteral("GrinderUnitTest-1"),lDependencies,GrinderUT::creator,GrinderUT::doInit,GrinderUT::start,GrinderUT::startInOwnThread);
}

void *GrinderUT::creator(void *pointerToAppObject)
{
    Q_UNUSED(pointerToAppObject)
    return &_gRegisterModuleLinker;
}

bool GrinderUT::startInOwnThread()
{ return false; }

bool GrinderUT::start(void *pointerToThis, void *pointerToAppObject)
{
    Q_UNUSED(pointerToThis)
    Q_UNUSED(pointerToAppObject)
    return true;
}

bool GrinderUT::doInit(void *pointerToThis, void *pointerToAppObject)
{
    Q_UNUSED(pointerToThis)

    bool            lResult     = true;
    App *           lApp        = reinterpret_cast<App *>(pointerToAppObject);
    Grinder *       lGrinder    = reinterpret_cast<Grinder *>(lApp->getModuleObject(QStringLiteral("Grinder-1")));

    if( lApp && lGrinder ) {
        qDebug() << "[BTC] Grinding 0.0001:";
        qDebug() << lGrinder->grindValues(QStringLiteral("0.0001"),QStringLiteral("BTC"));

        qDebug() << "[BTC] Grinding 0.0002:";
        qDebug() << lGrinder->grindValues(QStringLiteral("0.0002"),QStringLiteral("BTC"));

        qDebug() << "[BTC] Grinding 1:";
        qDebug() << lGrinder->grindValues(QStringLiteral("1"),QStringLiteral("BTC"));
    }

    return lResult;
}
