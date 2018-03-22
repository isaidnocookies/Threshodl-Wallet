#include "recordsmanager.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QDateTime>

RecordsManager::RecordsManager(const QString &iRecordsPath, QObject * iParent)
    : QObject( iParent )
    , mRecordsPath(iRecordsPath)
{
    QDir    lDir{mRecordsPath};

    if( ! lDir.exists() ) {
        if( ! lDir.mkpath(mRecordsPath) )
            qWarning() << "Unable to create records storage path" << mRecordsPath;
    }
}

void RecordsManager::threadStarted()
{

}

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
    }

    // Ignore it, not for us
}

void RecordsManager::saveDataBTCUSD(const QString iSource, const QByteArray iData)
{
    QString     lDirname    = QString("%1%2BTCUSD%3%4").arg(mRecordsPath).arg(QDir::separator()).arg(QDir::separator()).arg(iSource);
    QString     lFilename   = QString("%1%2%3.%4.BTCUSD").arg(lDirname).arg(QDir::separator()).arg(QDateTime::currentSecsSinceEpoch()).arg(iSource);
    QDir        lDir{lDirname};
    QFile       lFile{lFilename};

    if( ! lDir.exists() ) {
        if( ! lDir.mkpath(lDirname) ) {
            QByteArray lF = lFilename.toLocal8Bit(); QByteArray lD = lDirname.toLocal8Bit();
            qFatal( "Unable to create storage path for record for file '%s' ('%s')", lF.constData(), lD.constData() );
        }
    }

    if( lFile.open(QIODevice::WriteOnly) ) {
        if( lFile.write(iData) != iData.size() ) {
            qCritical() << "Possible corrupt write operation occurred when recording record file" << lFilename;
        }
        lFile.close();
    }else{
        QByteArray lF = lFilename.toLocal8Bit();
        qFatal( "Failed to open record file '%s' for writting!", lF.constData() );
    }
}

void RecordsManager::saveDataETHUSD(const QString iSource, const QByteArray iData)
{
    QString     lDirname    = QString("%1%2ETHUSD%3%4").arg(mRecordsPath).arg(QDir::separator()).arg(QDir::separator()).arg(iSource);
    QString     lFilename   = QString("%1%2%3.%4.ETHUSD").arg(lDirname).arg(QDir::separator()).arg(QDateTime::currentSecsSinceEpoch()).arg(iSource);
    QDir        lDir{lDirname};
    QFile       lFile{lFilename};

    if( ! lDir.exists() ) {
        if( ! lDir.mkpath(lDirname) ) {
            QByteArray lF = lFilename.toLocal8Bit(); QByteArray lD = lDirname.toLocal8Bit();
            qFatal( "Unable to create storage path for record for file '%s' ('%s')", lF.constData(), lD.constData() );
        }
    }

    if( lFile.open(QIODevice::WriteOnly) ) {
        if( lFile.write(iData) != iData.size() ) {
            qCritical() << "Possible corrupt write operation occurred when recording record file" << lFilename;
        }
        lFile.close();
    }else{
        QByteArray lF = lFilename.toLocal8Bit();
        qFatal( "Failed to open record file '%s' for writting!", lF.constData() );
    }
}

void RecordsManager::saveDataETHBTC(const QString iSource, const QByteArray iData)
{
    QString     lDirname    = QString("%1%2ETHBTC%3%4").arg(mRecordsPath).arg(QDir::separator()).arg(QDir::separator()).arg(iSource);
    QString     lFilename   = QString("%1%2%3.%4.ETHBTC").arg(lDirname).arg(QDir::separator()).arg(QDateTime::currentSecsSinceEpoch()).arg(iSource);
    QDir        lDir{lDirname};
    QFile       lFile{lFilename};

    if( ! lDir.exists() ) {
        if( ! lDir.mkpath(lDirname) ) {
            QByteArray lF = lFilename.toLocal8Bit(); QByteArray lD = lDirname.toLocal8Bit();
            qFatal( "Unable to create storage path for record for file '%s' ('%s')", lF.constData(), lD.constData() );
        }
    }

    if( lFile.open(QIODevice::WriteOnly) ) {
        if( lFile.write(iData) != iData.size() ) {
            qCritical() << "Possible corrupt write operation occurred when recording record file" << lFilename;
        }
        lFile.close();
    }else{
        QByteArray lF = lFilename.toLocal8Bit();
        qFatal( "Failed to open record file '%s' for writting!", lF.constData() );
    }
}
