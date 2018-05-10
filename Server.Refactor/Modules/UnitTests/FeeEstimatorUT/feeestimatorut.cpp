#include "feeestimatorut.h"
#include "../../Interface/FeeEstimatorInterface/feeestimatorinterface.h"
#include "app.h"
#include "modulelinker.h"

#include <QSignalSpy>

#ifdef UNIT_TEST_ON
static FeeEstimatorUT   _gRegisterModuleLinker;
#else
static void *           _gRegisterModuleLinker = nullptr; // To prevent compiler errors
#endif

FeeEstimatorUT::FeeEstimatorUT()
{
    QStringList lDependencies = QStringList() << QStringLiteral("Downloader-1") << QStringLiteral("RecordsManager-1") << QStringLiteral("FeeEstimator-1");
    ModuleLinker::registerModuleWithDependencies(QStringLiteral("FeeEstimatorUnitTest-1"),lDependencies,FeeEstimatorUT::creator,FeeEstimatorUT::doInit,FeeEstimatorUT::start,FeeEstimatorUT::startInOwnThread);
}

void *FeeEstimatorUT::creator(void *pointerToAppObject)
{
    Q_UNUSED(pointerToAppObject)
    return &_gRegisterModuleLinker;
}

bool FeeEstimatorUT::startInOwnThread()
{ return false; }

bool FeeEstimatorUT::doInit(void *pointerToThis, void *pointerToAppObject)
{
    Q_UNUSED(pointerToThis)
    Q_UNUSED(pointerToAppObject)
    return true;
}

bool FeeEstimatorUT::start(void *pointerToThis, void *pointerToAppObject)
{
    Q_UNUSED(pointerToThis)

    App *                   lApp    = reinterpret_cast<App *>(pointerToAppObject);
    FeeEstimatorInterface * lFE     = reinterpret_cast<FeeEstimatorInterface *>(lApp->getModuleObject(QStringLiteral("FeeEstimator-1")));

    if( lApp && lFE ) {

        auto lConfig = lApp->configuration();

        // Skip FeeEstimatorUT if told too
        if( lConfig->contains(QStringLiteral("FeeEstimatorUT")) ) {
            QString lValue = lConfig->toString(QStringLiteral("FeeEstimatorUT"));
            lConfig->removeValue(QStringLiteral("FeeEstimatorUT"));
            if( lValue == QStringLiteral("Skip") )
                return true;
        }

        QSignalSpy      lSignalSpy(lFE, SIGNAL(recordFee(QString,QString,QString)));

        // Need to download a list of crypto currencies and then check to make sure we get we have downloaded each one
        // Before saying the module is good

        if( ! lSignalSpy.wait(30000) ) {
            qWarning() << "Timed out waiting for a fee estimation download!";
            return false;
        }

        return true;
    }

    return false;
}
