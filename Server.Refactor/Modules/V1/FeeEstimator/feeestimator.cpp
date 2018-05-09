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

    QUrl lBTCTestNetFSource = QUrl::fromUserInput(lConfig->toString(QStringLiteral("BTCTestNetFeeEstimationSource")));
    QUrl lBTCMainNetFSource = QUrl::fromUserInput(lConfig->toString(QStringLiteral("BTCMainNetFeeEstimationSource")));

    if( lBTCTestNetFSource.isEmpty() || lBTCMainNetFSource.isEmpty() )
        return false;

    lFE->mUrlToCryptoCurrencyAndType[lBTCTestNetFSource] = QPair< QString, QString >( QStringLiteral("BTC"), QStringLiteral("TestNet") );
    lFE->mUrlToCryptoCurrencyAndType[lBTCMainNetFSource] = QPair< QString, QString >( QStringLiteral("BTC"), QStringLiteral("MainNet") );

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
{
}

FeeEstimator::~FeeEstimator()
{
}

void FeeEstimator::downloaded(const QUrl iUrl, const QByteArray iData)
{
    if( mUrlToCryptoCurrencyAndType.contains(iUrl) ) {
        // Its for us
        QPair< QString, QString >   lCryptoCurrencyAndType  = mUrlToCryptoCurrencyAndType[iUrl.toString()];
        QString                     lCryptoCurrency         = lCryptoCurrencyAndType.first;
        QString                     lType                   = lCryptoCurrencyAndType.second;
        QString                     lValuePerByte;
        QJsonParseError             lJsonError;
        QVariantMap                 lJsonMap                = QJsonDocument::fromJson(iData,&lJsonError).toVariant().toMap();

        if( lJsonError.error == QJsonParseError::NoError  && lJsonMap.size() > 0 ) {
            lValuePerByte = _divideStringValue(lJsonMap[lJsonMap.keys().first()].toString(),1000);
            emit recordFee(lCryptoCurrency,lType,lValuePerByte);

            if( lCryptoCurrency.toLower() == QStringLiteral("btc") ) {
                lType = lType.toLower();
                QMap< QString, QString >    lFees;
                double                      lValue = lValuePerByte.toDouble();

                if( lType == QStringLiteral("testnet") ) {
                    lFees[commonTypeToString(CommonFeeType::TestNetBaseFee)] = lValue * 10.0f;
                    lFees[commonTypeToString(CommonFeeType::TestNetInputFee)] = lValue * 180.0f;
                    lFees[commonTypeToString(CommonFeeType::TestNetOutputFee)] = lValue * 34.0f;
                    setFees(QStringLiteral("BTC"),lFees);
                }else if( lType == QStringLiteral("mainnet") ) {
                    lFees[commonTypeToString(CommonFeeType::MainNetBaseFee)] = lValue * 10.0f;
                    lFees[commonTypeToString(CommonFeeType::MainNetInputFee)] = lValue * 180.0f;
                    lFees[commonTypeToString(CommonFeeType::MainNetOutputFee)] = lValue * 34.0f;
                    setFees(QStringLiteral("BTC"),lFees);
                }else{
                    qWarning() << "Unhandled BTC chain type:" << lType;
                }
            }else{
                qWarning() << "Unknown crypto currency type:" << lCryptoCurrency;
            }
        }else{
            qWarning() << "Remote fee data parsing error!";
        }
    }else{
        qDebug() << "Fee estimator ignoring Url data from url:" << iUrl;
    }
}
