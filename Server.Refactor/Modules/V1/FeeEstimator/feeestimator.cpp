#include "feeestimator.h"
#include "app.h"
#include "modulelinker.h"

#include "../../Interface/RecordsManagerInterface/recordsmanagerinterface.h"
#include "../../Interface/DownloaderInterface/downloaderinterface.h"
//#include "../RecordsManager/recordsmanager.h"
//#include "../Downloader/downloader.h"

static FeeEstimatorML _gRegisterModuleLinker;

FeeEstimatorML::FeeEstimatorML()
{
    QStringList lDependencies = QStringList() << QStringLiteral("Downloader-1") << QStringLiteral("RecordsManager-1");
    ModuleLinker::registerModuleWithDependencies(QStringLiteral("FeeEstimator-1"),lDependencies,FeeEstimatorML::creator,FeeEstimatorML::doInit,FeeEstimatorML::start,FeeEstimatorML::startInOwnThread);
}

void *FeeEstimatorML::creator(void *pointerToAppObject)
{
    FeeEstimator * lFE  = new FeeEstimator;
    lFE->mApp           = reinterpret_cast<App *>(pointerToAppObject);
    return lFE;
}

bool FeeEstimatorML::doInit(void *pointerToThis, void *pointerToAppObject)
{
    Q_UNUSED(pointerToThis)
    Q_UNUSED(pointerToAppObject)
    return true;
}

bool FeeEstimatorML::startInOwnThread()
{ return false; }

bool FeeEstimatorML::start(void *pointerToThis, void *pointerToAppObject)
{
    Q_UNUSED(pointerToAppObject)
    FeeEstimator *              lFE = reinterpret_cast<FeeEstimator *>(pointerToThis);
    RecordsManagerInterface *   lRM = reinterpret_cast<RecordsManagerInterface *>(lFE->mApp->getModuleObject(QStringLiteral("RecordsManager-1")));
    DownloaderInterface *       lD  = reinterpret_cast<DownloaderInterface *>(lFE->mApp->getModuleObject(QStringLiteral("Downloader-1")));

    QObject::connect( lD, &DownloaderInterface::downloaded, lFE, &FeeEstimator::downloaded );


    return true;
}

FeeEstimator::FeeEstimator(QObject * iParent)
    : FeeEstimatorInterface(iParent)
{
}

FeeEstimator::~FeeEstimator()
{
}
