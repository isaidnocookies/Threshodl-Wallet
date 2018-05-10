#include "feeestimator.h"
#include "app.h"
#include "modulelinker.h"
#include "qstringmath.h"

#include "../../Interface/RecordsManagerInterface/recordsmanagerinterface.h"
#include "../../Interface/DownloaderInterface/downloaderinterface.h"

#include <QDebug>
#include <QJsonDocument>

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

    QVariantMap     lTestNetConf;
    lTestNetConf["url"]     = QStringLiteral("https://test-insight.bitpay.com/api/utils/estimatefee?nbBlocks=6");
    lTestNetConf["crypto"]  = QStringLiteral("BTC");
    lTestNetConf["chain"]   = QStringLiteral("TestNet");
    lTestNetConf["divisor"] = 1000;

    QVariantMap     lMainNetConf;
    lMainNetConf["url"]     = QStringLiteral("https://insight.bitpay.com/api/utils/estimatefee?nbBlocks=6");
    lMainNetConf["crypto"]  = QStringLiteral("BTC");
    lMainNetConf["chain"]   = QStringLiteral("MainNet");
    lMainNetConf["divisor"] = 1000;

    lConfig->setValue(QStringLiteral("FeeEstimationSources"), QVariantList() << lTestNetConf << lMainNetConf);

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

    for( QVariant lE : lConfig->toVariantList(QStringLiteral("FeeEstimationSources")) )
    {
        QVariantMap lM = lE.toMap();

        if( lM.isEmpty() || ! lM.contains(QStringLiteral("url")) || ! lM.contains(QStringLiteral("crypto")) )
            continue;

        if( ! lM.contains(QStringLiteral("divisor")) )
            lM[QStringLiteral("divisor")] = 1000;

        if( ! lM.contains(QStringLiteral("chain")) && lM["crypto"] == QStringLiteral("BTC") )
            lM[QStringLiteral("chain")] = QStringLiteral("MainNet");

        lFE->mCryptoSources << lM;
        lFE->mUrlToCryptoSource[QUrl(lM[QStringLiteral("url")].toString())] = lM;
    }

    if( lFE->mCryptoSources.isEmpty() || lFE->mUrlToCryptoSource.isEmpty() )
        return false;

    return true;
}

bool FeeEstimator::_normalizeString(const QString iInput, QString &oOutput, bool &oIsNegative)
{
    oIsNegative = false;
    oOutput     = iInput.trimmed();

    if( oOutput.isEmpty() )
        return false;

    if( oOutput.startsWith(QChar{'-'}) ) {
        oIsNegative = true;
        oOutput = oOutput.mid(1).trimmed();
    }else if( oOutput.startsWith(QChar{'+'}) ) {
        oIsNegative = false;
        oOutput = oOutput.mid(1).trimmed();
    }

    if( oOutput.startsWith(QChar{'.'}) )
        oOutput.prepend(QChar{'0'});

    if( ! oOutput.contains(QChar{'.'}) )
        oOutput.append(QStringLiteral(".0"));

    return true;
}

QString FeeEstimator::_divideStringValue(const QString iValue, unsigned int iDivsor)
{
    QString         lValue;
    QString         lResult;
    bool            lIsNegative;
    QString         lValueLeftString;
    QString         lValueRightString;
    unsigned int    lModuloLeft;
    unsigned int    lModuloRight;
    int             lDecPos;
    unsigned int    lWorkingValue;
    bool            lOk;
    unsigned int    lModuloBalance;
    int             lModLimitor;

    try {
        if( ! _normalizeString(iValue,lValue,lIsNegative) )  throw __LINE__;

        lDecPos = lValue.indexOf(QChar{'.'});
        if( lDecPos < 0 ) throw __LINE__;

        lValueLeftString = lValue.left(lDecPos);
        lValueRightString = lValue.mid(lDecPos+1); // Skip '.'

        lWorkingValue = lValueLeftString.toUInt(&lOk,10);
        if( ! lOk ) throw __LINE__;

        lModuloLeft         = (lWorkingValue % iDivsor) * (10 * lValueRightString.length());
        lWorkingValue       = lWorkingValue / iDivsor;
        lValueLeftString    = QString::number(lWorkingValue,10);

        lWorkingValue = lValueRightString.toUInt(&lOk,10);
        if( ! lOk ) throw __LINE__;

        lModuloRight        = (lWorkingValue % iDivsor) + (lModuloLeft % iDivsor);
        lWorkingValue       = (lWorkingValue / iDivsor) + (lModuloLeft / iDivsor);

        lResult             = QStringLiteral("%1.%2").arg(lValueLeftString).arg(lWorkingValue,lValueRightString.length(),10,QChar{'0'});

        for( lModLimitor = 0; lModLimitor < 16 && lModuloRight > 0; lModLimitor++ ) {
            lModuloRight = lModuloRight * 10;
            lModuloBalance = lModuloRight % iDivsor;
            lWorkingValue = lModuloRight / iDivsor;
            lResult = lResult.append(QString::number(lWorkingValue,10));
            lModuloRight = lModuloBalance;
        }

        if( lIsNegative )
            return QStringLiteral("-%1").arg(lResult);
        return lResult;
    } catch(int iLine) {
        qDebug() << "Error line" << iLine;
        return QStringLiteral("0.0");
    }
}

FeeEstimator::FeeEstimator(QObject * iParent)
    : FeeEstimatorInterface(iParent)
{ }

FeeEstimator::~FeeEstimator()
{ }

void FeeEstimator::downloaded(const QUrl iUrl, const QByteArray iData)
{
    if( mUrlToCryptoSource.contains(iUrl) ) {
        // Its for us
        QJsonParseError             lJsonError;
        QString                     lValue;
        QMap< QString, QString >    lFees;
        double                      lValueD;
        QVariantMap                 lCryptoSource   = mUrlToCryptoSource[iUrl];
        QString                     lCrypto         = lCryptoSource[QStringLiteral("crypto")].toString();
        QString                     lChain          = lCryptoSource[QStringLiteral("chain")].toString();
        QVariantMap                 lJsonMap        = QJsonDocument::fromJson(iData,&lJsonError).toVariant().toMap();

        if( lJsonError.error == QJsonParseError::NoError  && lJsonMap.size() > 0 ) {
            lValue  = _divideStringValue(lJsonMap[lJsonMap.keys().first()].toString(),lCryptoSource[QStringLiteral("divisor")].toInt());
            if( lCrypto.toLower() == QStringLiteral("btc") ) {

                emit recordFee(lCrypto,lChain,lValue);

                lValueD = lValue.toDouble();

                if( lChain == QStringLiteral("testnet") ) {
                    lFees[commonTypeToString(CommonFeeType::TestNetBaseFee)] = lValueD * 10.0f;
                    lFees[commonTypeToString(CommonFeeType::TestNetInputFee)] = lValueD * 180.0f;
                    lFees[commonTypeToString(CommonFeeType::TestNetOutputFee)] = lValueD * 34.0f;
                    setFees(lCrypto,lFees);
                }else if( lChain == QStringLiteral("mainnet") ) {
                    lFees[commonTypeToString(CommonFeeType::MainNetBaseFee)] = lValueD * 10.0f;
                    lFees[commonTypeToString(CommonFeeType::MainNetInputFee)] = lValueD * 180.0f;
                    lFees[commonTypeToString(CommonFeeType::MainNetOutputFee)] = lValueD * 34.0f;
                    setFees(lCrypto,lFees);
                }else{
                    qWarning() << "Unhandled BTC chain type:" << lChain;
                }
            }else{
                qWarning() << "Unknown crypto currency type:" << lCrypto;
            }
        }else{
            qWarning() << "Remote fee data parsing error!";
        }
    }else{
        qDebug() << "Fee estimator ignoring Url data from url:" << iUrl;
    }
}
