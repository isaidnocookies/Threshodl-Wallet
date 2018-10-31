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
        QThread::sleep(POLLING_INTERVAL);
    }

    emit finished();
}

void DownloadWorker::stopDownloading()
{
    mDownloading = false;
}

void DownloadWorker::setAddresses(QString iShortname, QStringList iAddresses)
{
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

    //https://_pyts.threebx.com/ex/rates
    auto lRequestURL = QUrl(QString("%1/%2").arg(TASK_SERVER_URL).arg("ex/rates"));
    qDebug() << lRequestURL;

    lReply = mNetworkManager->get(QNetworkRequest(lRequestURL));
    lMyEventLoop.exec();

    if (lReply->error() == QNetworkReply::NoError) {
        QByteArray      lReplyText = lReply->readAll();
        auto            lMyMap = QJsonDocument::fromJson(lReplyText).toVariant().toMap();
        QMap<QString, QString> lUpdatedValues;
        QStringList lNames, lValues;

        for (auto coin : AppWallets::walletNames().keys()) {
            lNames.append(coin);
            lValues.append(lMyMap[(coin.at(0) == "t" ? coin.remove(0, 1) : coin)].toMap()["USD"].toMap()["rate"].toString());
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
        if (addressKey == "")
            continue;

        if (addressKey.at(0) == "d") {
            //Skip dark wallets
            qDebug() << "Skipping: " << addressKey;
            continue;
        }

        if (addressKey != "DOGE" && addressKey != "tDOGE" && addressKey != "ETH" && addressKey != "tETH" && addressKey != "tLTC") {
            downloadGenericInsightBalances(addressKey, mAddressesToCheck[addressKey]);
        } else if (addressKey == "tLTC" || addressKey == "DOGE" || addressKey == "tDOGE") {
            downloadSoChainBalances(addressKey, mAddressesToCheck[addressKey]);
        }
    }
}

void DownloadWorker::downloadSoChainBalances(QString iShortname, QStringList iAddresses)
{
    QNetworkAccessManager   *mNetworkManager = new QNetworkAccessManager(this);
    QEventLoop              lMyEventLoop;
    QNetworkReply           *lReply;

    connect(mNetworkManager, SIGNAL(finished(QNetworkReply*)), &lMyEventLoop, SLOT(quit()));

    QStringList lAddresses;
    QStringList lBalances;
    QStringList lPendingBalances;
    QString lCoinNetwork = "";

    if (iShortname == "tLTC") {
        lCoinNetwork = "LTCTEST";
    } else if (iShortname == "DOGE") {
        lCoinNetwork = "DOGE";
    } else if(iShortname == "tDOGE") {
        lCoinNetwork = "DOGETEST";
    } else {
        qDebug() << "Error.... Can not update bright wallet balance...";
        return;
    }

    for (auto addr : iAddresses) {
        auto lRequestURL = QUrl(QString("%1/get_address_balance/%2/%3").arg(EndpointUrls::insightUrls()[iShortname]).arg(lCoinNetwork).arg(QString(addr)));
        lReply = mNetworkManager->get(QNetworkRequest(lRequestURL));
        lMyEventLoop.exec();

        if (lReply->error() == QNetworkReply::NoError) {
            QByteArray      lReplyText = lReply->readAll();
            auto            lMyMap = QJsonDocument::fromJson(lReplyText).toVariant().toMap();

            qDebug() << lRequestURL;

            if (lMyMap["status"].toString() != "success") {
                qDebug() << "Error.... Can not update bright wallet balance...";
                break;
            }

            QStringMath     lBalance = lMyMap["confirmed_balance"].toString();
            QString         lPendingString = lMyMap["unconfirmed_balance"].toString();
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

void DownloadWorker::downloadGenericInsightBalances(QString iShortname, QStringList iAddresses)
{
    QNetworkAccessManager   *mNetworkManager = new QNetworkAccessManager(this);
    QEventLoop              lMyEventLoop;
    QNetworkReply           *lReply;

    connect(mNetworkManager, SIGNAL(finished(QNetworkReply*)), &lMyEventLoop, SLOT(quit()));

    QStringList lAddresses;
    QStringList lBalances;
    QStringList lPendingBalances;

    for (auto addr : iAddresses) {
        auto lRequestURL = QUrl(QString("%1/addr/%2").arg(EndpointUrls::insightUrls()[iShortname]).arg(QString(addr)));
        lReply = mNetworkManager->get(QNetworkRequest(lRequestURL));
        lMyEventLoop.exec();

        if (lReply->error() == QNetworkReply::NoError) {
            QByteArray      lReplyText = lReply->readAll();
            auto            lMyMap = QJsonDocument::fromJson(lReplyText).toVariant().toMap();

            qDebug() << lRequestURL;

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
