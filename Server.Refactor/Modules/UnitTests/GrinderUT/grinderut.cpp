#include "grinderut.h"
#include "modulelinker.h"
#include "app.h"
#include "../../V1/Grinder/grinder.h"
#include "qstringmath.h"

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

inline static bool __TestGrinder(const QString iValue, Grinder * iGrinder, QString iCryptoCurrency = QStringLiteral("BTC"))
{
    qDebug().noquote().nospace() << "[" << iCryptoCurrency << "] Grinding " << iValue << ":";
    QStringList     lValues = iGrinder->grindValues(iValue,iCryptoCurrency);
    QStringMath     lBefore{iValue};
    QStringMath     lAfter{QStringLiteral("0.0")};

    for( QString lE : lValues )
    { lAfter = lAfter.add(lE); }

    if( lAfter == lBefore ) {
        qDebug().noquote().nospace() << "SUCCESSFUL, Generated " << lValues.size() << " values totaling " << lAfter.toString() << " matching " << lBefore.toString();
        return true;
    }

    qDebug().noquote().nospace() << "FAILED, Generated " << lValues.size() << " values totaling " << lAfter.toString() << " instead of " << lBefore.toString();
    return false;
}

bool GrinderUT::doInit(void *pointerToThis, void *pointerToAppObject)
{
    Q_UNUSED(pointerToThis)

    App *           lApp        = reinterpret_cast<App *>(pointerToAppObject);
    Grinder *       lGrinder    = reinterpret_cast<Grinder *>(lApp->getModuleObject(QStringLiteral("Grinder-1")));
    QStringList     lTestList;

    if( lApp && lGrinder ) {

        qDebug() << "Generating test list, this will take a moment...";

        for( int lHigh = 0; lHigh <= 10; lHigh++ )
        {
            for( int lLow = 0; lLow <= 9999; lLow++ )
            {
                lTestList << QStringLiteral("%1.%2").arg(lHigh).arg(lLow,4,10,QChar{'0'});
            }
        }

        qDebug() << "Test range set to" << lTestList.first() << "through" << lTestList.last();

        for( QString lE : lTestList )
        {
            if( ! __TestGrinder(lE,lGrinder) ) {
                return false;
            }
        }

        return true;
    }

    return false;
}
