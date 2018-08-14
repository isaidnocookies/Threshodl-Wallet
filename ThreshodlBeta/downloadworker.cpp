#include "downloadworker.h"

#include <QEventLoop>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QThread>
#include <QDebug>
#include <QMutex>
#include <QJsonDocument>

DownloadWorker::DownloadWorker(QWaitCondition *iWaitCondition, QMutex *iWaitMutex)
{
    mWaitCondition = iWaitCondition;
    mWaitMutex = iWaitMutex;
}

DownloadWorker::~DownloadWorker()
{
    mDownloading = false;
}

void DownloadWorker::startDownloading()
{
    mDownloading = true;
    QThread::sleep(5);
    qDebug() << "Downloader Started...";

    while (mDownloading) {
        downloadMarketValues();
        downloadBalances();
        QThread::sleep(60);
    }

    emit finished();
}

void DownloadWorker::stopDownloading()
{
    mDownloading = false;
}

void DownloadWorker::setAddresses(QString iShortname, QStringList iAddresses)
{
    qDebug() << "setAddresses() : " << iShortname << "Addresses : " << iAddresses;
    if (iShortname != "") {
        mAddressesToCheck[iShortname] = iAddresses;
    } else {
        qDebug() << "Attempted to add empty wallet to downloader avoided... Nice";
    }
}

void DownloadWorker::downloadMarketValues()
{
    QNetworkAccessManager   *mNetworkManager = new QNetworkAccessManager(this);
    QEventLoop              lMyEventLoop;
    QNetworkReply           *lReply;
    connect(mNetworkManager, SIGNAL(finished(QNetworkReply*)), &lMyEventLoop, SLOT(quit()));

    //https://ts.threebx.com/a/exchangerate/?btc:usd&ltc:usd

    QString lRequestParameter = "?";
    for (auto sn : AppWallets::walletNames().keys()) {
        if (sn.at(0) == "d") {
            continue;
        } else if (sn.at(0) == "t") {
            continue;
        }
        lRequestParameter = lRequestParameter.append( QString("USD:%1&").arg(QString(sn)));
    }
    lRequestParameter.remove(lRequestParameter.size() - 1, 1);
    auto lRequestURL = QUrl(QString("%1/%2").arg(TASK_SERVER_URL).arg(lRequestParameter));
    qDebug() << lRequestURL;

    lReply = mNetworkManager->get(QNetworkRequest(lRequestURL));
    lMyEventLoop.exec();

    if (lReply->error() == QNetworkReply::NoError) {
        QByteArray      lReplyText = lReply->readAll();
        auto            lMyMap = QJsonDocument::fromJson(lReplyText).toVariant().toMap();
        QMap<QString, QString> lUpdatedValues;
        QStringList lNames, lValues;

        for (auto key : lMyMap.keys()) {
            auto lKeyEntry = lMyMap[key].toMap();
            auto lResponseKey = lKeyEntry["last"].toMap();
            lUpdatedValues[QString(key).remove("USD")] = lResponseKey["rate"].toString();

            lNames.append(key.remove("USD"));
            lValues.append(lResponseKey["rate"].toString());
        }
        emit allMarketValuesUpdated(lNames, lValues);
    } else {
        qDebug() << "Error.... Can not update market values...";
    }
}

void DownloadWorker::downloadBalances()
{
    QMap<QString, QStringList> lAddressMap = mAddressesToCheck;
    qDebug() << "Downloading Balances";

    for (auto addressKey : lAddressMap.keys()) {
        qDebug() << "downloading for " << addressKey;

        if (addressKey == "") {
            continue;
        }

        if (addressKey.at(0) == "d") {
            //Skip dark wallets
            qDebug() << "Skipping: " << addressKey;
            continue;
        }

        if (addressKey != "DOGE" || addressKey != "tDOGE" || addressKey != "ETH" || addressKey != "tETH") {
            downloadGenericInsightBalances(addressKey, mAddressesToCheck[addressKey]);
        }
    }
}

void DownloadWorker::downloadGenericInsightBalances(QString iShortname, QStringList iAddresses)
{
    qDebug() << "Downloading Generic Insight Balance";

    QNetworkAccessManager   *mNetworkManager = new QNetworkAccessManager(this);
    QEventLoop              lMyEventLoop;
    QNetworkReply           *lReply;

    connect(mNetworkManager, SIGNAL(finished(QNetworkReply*)), &lMyEventLoop, SLOT(quit()));

    QStringList lAddresses;
    QStringList lBalances;
    QStringList lPendingBalances;

    for (auto addr : iAddresses) {
        auto lRequestURL = QUrl(QString("%1/addr/%2").arg(EndpointUrls::insightUrls()[iShortname]).arg(QString(addr)));
        qDebug() << lRequestURL;
        lReply = mNetworkManager->get(QNetworkRequest(lRequestURL));
        lMyEventLoop.exec();

        if (lReply->error() == QNetworkReply::NoError) {
            QByteArray      lReplyText = lReply->readAll();
            auto            lMyMap = QJsonDocument::fromJson(lReplyText).toVariant().toMap();

            QStringMath     lBalance = lMyMap["balance"].toString();
            QString         lPendingString = lMyMap["unconfirmedBalance"].toString();
            QStringMath     lPendingBalance = QStringMath("0.0");

            if (!lPendingString.isEmpty()) {
                if (lPendingString.at(0) == "-") {
                    lPendingString.remove(0,1);
                    lPendingBalance = lBalance - lMyMap["unconfirmedBalance"].toString();
                } else {
                    lPendingBalance = lBalance + lMyMap["unconfirmedBalance"].toString();
                }
            }
            qDebug() << iShortname << " -- " << addr << " -- " << "Balance: " << lBalance.toString() << "  Pending: " << lPendingBalance.toString();

            lAddresses << addr;
            lBalances << lBalance.toString();
            lPendingBalances << lPendingBalance.toString();
        } else {
            qDebug() << "Error.... Can not update bright wallet balance...";
        }
    }
    emit walletAddressesUpdated(iShortname, lAddresses, lBalances, lPendingBalances);
}
