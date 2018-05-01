#include "recordsmanager.h"
#include "modulelinker.h"
#include "app.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QDateTime>

static RecordsManagerML _gRegisterModuleLinker;

RecordsManagerML::RecordsManagerML()
{
    ModuleLinker::registerModule(QStringLiteral("RecordsManager-1"),RecordsManagerML::creator,RecordsManagerML::doInit,RecordsManagerML::start,RecordsManagerML::startInOwnThread);
}

void *RecordsManagerML::creator(void *pointerToAppObject)
{
    App *               lApp            = reinterpret_cast<App *>(pointerToAppObject);
    QString             lRecordsPath    = lApp->configuration()->toString(QStringLiteral("RecordsPath"));
    RecordsManager *    lRM             = new RecordsManager{lRecordsPath};
    lRM->mApp                           = lApp;
    return lRM;
}

bool RecordsManagerML::doInit(void *pointerToThis, void *pointerToAppObject)
{
    Q_UNUSED(pointerToAppObject)
    RecordsManager *    lRM = reinterpret_cast<RecordsManager *>(pointerToThis);
    if( lRM ) {
        return lRM->doInit();
    }
    return false;
}

bool RecordsManagerML::startInOwnThread()
{ return false; }

bool RecordsManagerML::start(void *pointerToThis, void *pointerToAppObject)
{
    Q_UNUSED(pointerToAppObject)
    RecordsManager *    lRM = reinterpret_cast<RecordsManager *>(pointerToThis);
    if( lRM ) {
        lRM->threadStarted();
        return true;
    }
    return false;
}

void RecordsManager::_recordFileAndStoreLocalVariable(const QString iPath, const QString iFilename, const QByteArray iData, QByteArray &oLocalVariable)
{
    QDir        lDir{iPath};
    QFile       lFile{iFilename};

    if( ! lDir.exists() ) {
        if( ! lDir.mkpath(iPath) ) {
            QByteArray lF = iFilename.toLocal8Bit(); QByteArray lD = iPath.toLocal8Bit();
            qFatal( "Unable to create storage path for record for file '%s' ('%s')", lF.constData(), lD.constData() );
        }
    }

    if( lFile.open(QIODevice::WriteOnly) ) {
        if( lFile.write(iData) != iData.size() ) {
            qCritical() << "Possible corrupt write operation occurred when recording record file" << iFilename;
        }else{
            oLocalVariable = iData;
        }
        lFile.close();
    }else{
        QByteArray lF = iFilename.toLocal8Bit();
        qFatal( "Failed to open record file '%s' for writting!", lF.constData() );
    }
}

RecordsManager::RecordsManager(const QString &iRecordsPath, QObject * iParent)
    : RecordsManagerInterface( iParent )
    , mRecordsPath(iRecordsPath)
{
    QDir    lDir{mRecordsPath};

    if( ! lDir.exists() ) {
        if( ! lDir.mkpath(mRecordsPath) )
            qWarning() << "Unable to create records storage path" << mRecordsPath;
    }
}

QByteArray RecordsManager::lastDataBTCUSD() const
{ return mDataBTCUSD; }

QByteArray RecordsManager::lastDataETHUSD() const
{ return mDataETHUSD; }

QByteArray RecordsManager::lastDataETHBTC() const
{ return mDataETHBTC; }

QByteArray RecordsManager::testNetEstimateFee() const
{ return mTestNetEstimateFee; }

bool RecordsManager::doInit()
{
    QString lRecordsPath = mApp->configuration()->toString(QStringLiteral("RecordsPath"));
    QDir    lDir{lRecordsPath};

    if( ! lDir.exists() ) {
        if( ! lDir.mkpath(lRecordsPath) ) {
            qWarning() << "Unable to create records storage path" << lRecordsPath;
            return false;
        }
    }

    return true;
}

void RecordsManager::threadStarted()
{ }

void RecordsManager::handleDownloadedUrlData(const QString iUrl, const QByteArray iData)
{
    if( iUrl == QStringLiteral("https://apiv2.bitcoinaverage.com/indices/global/ticker/BTCUSD") ) {
        saveDataBTCUSD(QStringLiteral("bitcoinaverage-apiv2"),iData);
        return;
    } else if( iUrl == QStringLiteral("https://apiv2.bitcoinaverage.com/indices/global/ticker/ETHUSD") ) {
        saveDataETHUSD(QStringLiteral("bitcoinaverage-apiv2"),iData);
        return;
    } else if( iUrl == QStringLiteral("https://apiv2.bitcoinaverage.com/indices/crypto/ticker/ETHBTC") ) {
        saveDataETHBTC(QStringLiteral("bitcoinaverage-apiv2"),iData);
        return;
    } else if( iUrl.startsWith(QStringLiteral("https://test-insight.bitpay.com/api/utils/estimatefee?nbBlocks=")) ) {
        saveDataBTCTestNetBlockChainStats("test-insight.bitpay.com_api_utils_estimatefee",iData);
        return;
    }

    // Ignore it, not for us
}

void RecordsManager::saveDataBTCTestNetBlockChainStats(const QString iSource, const QByteArray iData)
{
    QString     lDirname    = QString("%1%2testnet_estimatefee%3%4").arg(mRecordsPath).arg(QDir::separator()).arg(QDir::separator()).arg(iSource);
    QString     lFilename   = QString("%1%2%3.%4.testnet_estimatefee").arg(lDirname).arg(QDir::separator()).arg(QDateTime::currentSecsSinceEpoch()).arg(iSource);
    _recordFileAndStoreLocalVariable( lDirname, lFilename, iData, mTestNetEstimateFee );
    emit btcTestNetEstimateFeesChanged();
}

void RecordsManager::saveDataBTCUSD(const QString iSource, const QByteArray iData)
{
    QString     lDirname    = QString("%1%2BTCUSD%3%4").arg(mRecordsPath).arg(QDir::separator()).arg(QDir::separator()).arg(iSource);
    QString     lFilename   = QString("%1%2%3.%4.BTCUSD").arg(lDirname).arg(QDir::separator()).arg(QDateTime::currentSecsSinceEpoch()).arg(iSource);
    _recordFileAndStoreLocalVariable( lDirname, lFilename, iData, mDataBTCUSD );
}

void RecordsManager::saveDataETHUSD(const QString iSource, const QByteArray iData)
{
    QString     lDirname    = QString("%1%2ETHUSD%3%4").arg(mRecordsPath).arg(QDir::separator()).arg(QDir::separator()).arg(iSource);
    QString     lFilename   = QString("%1%2%3.%4.ETHUSD").arg(lDirname).arg(QDir::separator()).arg(QDateTime::currentSecsSinceEpoch()).arg(iSource);
    _recordFileAndStoreLocalVariable( lDirname, lFilename, iData, mDataETHUSD );
}

void RecordsManager::saveDataETHBTC(const QString iSource, const QByteArray iData)
{
    QString     lDirname    = QString("%1%2ETHBTC%3%4").arg(mRecordsPath).arg(QDir::separator()).arg(QDir::separator()).arg(iSource);
    QString     lFilename   = QString("%1%2%3.%4.ETHBTC").arg(lDirname).arg(QDir::separator()).arg(QDateTime::currentSecsSinceEpoch()).arg(iSource);
    _recordFileAndStoreLocalVariable( lDirname, lFilename, iData, mDataETHBTC );
}
