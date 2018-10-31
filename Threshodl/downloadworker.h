#ifndef DOWNLOADWORKER_H
#define DOWNLOADWORKER_H

#include "core.h"

#include <QMutex>
#include <QObject>
#include <QNetworkAccessManager>
#include <QWaitCondition>

class DownloadWorker : public QObject
{
    Q_OBJECT
public:
    DownloadWorker(QWaitCondition *iWaitCondition, QMutex *iWaitMutex);
    ~DownloadWorker();
signals:
    void finished();
    void error(QString err);
    void marketValueUpdated(QString oShortname, QString oValue);
    void allMarketValuesUpdated(QStringList oNames, QStringList oValues);
    void walletAddressesUpdated(QString oShortname, QStringList oAddresses, QStringList oBalances, QStringList oPendingBalances);
    void balancesUpdated();

public slots:
    void startDownloading();
    void stopDownloading();

    void setAddresses(QString iShortname, QStringList iAddresses);

private:
    bool                        mDownloading;
    QMap<QString, QStringList>  mAddressesToCheck;
    QMutex                      mAddressMapMutex;

    QWaitCondition              *mWaitCondition;
    QMutex                      *mWaitMutex;

    void downloadMarketValues();
    void downloadBalances();

    void downloadBitcoinBalances(QStringList iAddresses);
    void downloadGenericInsightBalances(QString iShortname, QStringList iAddresses);
    void downloadSoChainBalances(QString iShortname, QStringList iAddresses);

};

#endif // DOWNLOADWORKER_H
