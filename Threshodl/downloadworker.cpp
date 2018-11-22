#include "downloadworker.h"

#include <QEventLoop>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QThread>
#include <QDebug>
#include <QMutex>
#include <QJsonDocument>
#include <QJsonObject>

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

        downloadBalances(addressKey, mAddressesToCheck[addressKey]);
    }
}

void DownloadWorker::downloadBalances(QString iShortname, QStringList iAddresses)
{
    QNetworkAccessManager   *mNetworkManager = new QNetworkAccessManager(this);
    QEventLoop              lMyEventLoop;
    QNetworkReply           *lReply;

    QStringList lAddresses;
    QStringList lBalances;
    QStringList lPendingBalances;

    connect(mNetworkManager, SIGNAL(finished(QNetworkReply*)), &lMyEventLoop, SLOT(quit()));

    for (auto addr : iAddresses) {
        auto lRequestURL = QUrl(QString("%1/wallets/balance").arg(MY_WALLET_SERVER_ADDRESS));

        QJsonObject jsonData;
        jsonData.insert("coin", iShortname);
        jsonData.insert("address", addr);

        QJsonDocument jsonDataDocument;
        jsonDataDocument.setObject(jsonData);

        QByteArray request_body = jsonDataDocument.toJson();

        QNetworkRequest lRequest;
        lRequest.setUrl(lRequestURL);
        lRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

        lReply = mNetworkManager->post(lRequest, request_body);
        lMyEventLoop.exec();

        if (lReply->error() == QNetworkReply::NoError) {
            QByteArray      lReplyText = lReply->readAll();
            auto            lMyMap = QJsonDocument::fromJson(lReplyText).toVariant().toMap();

            if (!lMyMap["success"].toBool()) {
                qDebug() << "Error.... Failed response from wallet server for balance check... " << iShortname;
                return;
            }

            auto lResultMap = lMyMap["response"].toMap();
            QStringMath     lBalance = lResultMap["confirmed"].toString();
            QString         lPendingString = lResultMap["unconfirmed"].toString();
            QStringMath     lPendingBalance = QStringMath("0.0");

            if (!lPendingString.isEmpty()) {
                if (lPendingString.at(0) == "-") {
                    lPendingString.remove(0,1);
                    lPendingBalance = lBalance - lPendingString;
                } else {
                    lPendingBalance = lBalance + lPendingString;
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
