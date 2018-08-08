#ifndef DOWNLOADWORKER_H
#define DOWNLOADWORKER_H

#include <QObject>
#include <QNetworkAccessManager>

#include "globalsandconstants.h"

class DownloadWorker : public QObject
{
    Q_OBJECT
public:
    DownloadWorker();
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
    int mInterval;
    bool                        mDownloading;
    QMap<QString, QStringList>  mAddressesToCheck;
    QMutex                      mAddressMapMutex;

    void downloadMarketValues();
    void downloadBalances();

    void downloadBitcoinBalances(QStringList iAddresses);

};

#endif // DOWNLOADWORKER_H
