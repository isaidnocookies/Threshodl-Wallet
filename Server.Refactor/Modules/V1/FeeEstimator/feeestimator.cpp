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
    Q_UNUSED(pointerToAppObject)

    FeeEstimator *              lFE = reinterpret_cast<FeeEstimator *>(pointerToThis);
    auto *                      lConfig = lFE->mApp->configuration();

    lConfig->setValue(QStringLiteral("BTCTestNetFeeEstimationSource"),QStringLiteral("https://test-insight.bitpay.com/api/utils/estimatefee?nbBlocks=3"));
    lConfig->setValue(QStringLiteral("BTCMainNetFeeEstimationSource"),QStringLiteral("https://insight.bitpay.com/api/utils/estimatefee?nbBlocks=3"));

    return true;
}

bool FeeEstimatorML::startInOwnThread()
{ return false; }

bool FeeEstimatorML::start(void *pointerToThis, void *pointerToAppObject)
{
    Q_UNUSED(pointerToAppObject)
    FeeEstimator *              lFE = reinterpret_cast<FeeEstimator *>(pointerToThis);
    auto *                      lConfig = lFE->mApp->configuration();
    RecordsManagerInterface *   lRM = reinterpret_cast<RecordsManagerInterface *>(lFE->mApp->getModuleObject(QStringLiteral("RecordsManager-1")));
    DownloaderInterface *       lD  = reinterpret_cast<DownloaderInterface *>(lFE->mApp->getModuleObject(QStringLiteral("Downloader-1")));

    if( ! lRM || ! lD || ! lConfig )
        return false;

    QObject::connect( lD, &DownloaderInterface::downloaded, lFE, &FeeEstimator::downloaded );
    QObject::connect( lFE, &FeeEstimator::recordFee, lRM, &RecordsManagerInterface::recordFee );

    QString lBTCTestNetFSource = lConfig->toString(QStringLiteral("BTCTestNetFeeEstimationSource"));
    QString lBTCMainNetFSource = lConfig->toString(QStringLiteral("BTCMainNetFeeEstimationSource"));

    if( lBTCTestNetFSource.isEmpty() || lBTCMainNetFSource.isEmpty() )
        return false;

    lFE->mUrlToCryptoCurrencyAndType[lBTCTestNetFSource] = QPair< QString, QString >( QStringLiteral("BTC"), QStringLiteral("TestNet") );
    lFE->mUrlToCryptoCurrencyAndType[lBTCMainNetFSource] = QPair< QString, QString >( QStringLiteral("BTC"), QStringLiteral("MainNet") );

    return true;
}

FeeEstimator::FeeEstimator(QObject * iParent)
    : FeeEstimatorInterface(iParent)
{
}

FeeEstimator::~FeeEstimator()
{
}

void FeeEstimator::downloaded(const QUrl iUrl, const QByteArray iData)
{

}
