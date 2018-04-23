#include "bitcoinblockchaininterface.h"

extern "C" {
#include <btc/chainparams.h>
#include <btc/ecc.h>
#include <btc/tool.h>
#include <btc/utils.h>

#include <btc/tx.h>
#include <btc/cstr.h>
#include <btc/ecc_key.h>
#include <btc/script.h>
#include <btc/utils.h>

#include <btc/tool.h>
#include <btc/bip32.h>
}

#include <QEventLoop>

BitcoinBlockchainInterface::BitcoinBlockchainInterface(QObject *parent) : QObject(parent)
{
    mNetworkAccessManager = new QNetworkAccessManager(this);
}

void BitcoinBlockchainInterface::setActiveUser(UserAccount *iActiveUser)
{
    mActiveUser = iActiveUser;
}

bool BitcoinBlockchainInterface::updateBrightWalletBalances(int iConfirmations)
{
    QEventLoop              lMyEventLoop;
    QNetworkReply           *lReply;
    QStringMath             lBrightBalance = QStringMath();
    QList<BitcoinWallet>    lNewWallets;
    bool                    lSuccess = true;

    connect(mNetworkAccessManager, SIGNAL(finished(QNetworkReply*)), &lMyEventLoop, SLOT(quit()));

    for (BitcoinWallet &w : mActiveUser->getBrightWallets()) {
        lReply = mNetworkAccessManager->get(QNetworkRequest(QUrl(QString("%1/addrs/%2/balance").arg(BLOCKCYPER_API_URL).arg(QString(w.address())))));
        lMyEventLoop.exec();

        if (lReply->error() == QNetworkReply::NoError) {
            QByteArray      lReplyText = lReply->readAll();
            QJsonDocument   lJsonReponse = QJsonDocument::fromJson(lReplyText);
            QJsonObject     lJsonObject = lJsonReponse.object();
            QVariantMap     lMap = lJsonObject.toVariantMap();
            BitcoinWallet   lNewWallet = w;
            QStringMath     lBalance = QStringMath::btcFromSatoshi(QString::number(lMap["balance"].toInt()));
            lNewWallet.setValue(lBalance.toString());
            lNewWallets.append(lNewWallet);
            lBrightBalance = lBrightBalance + lBalance;
        } else {
            qDebug() << "Error.... ";
            lSuccess = false;
            lNewWallets.append(w);
        }
    }

    mActiveUser->setBrightWallets(lNewWallets);

    return lSuccess;
}

bool BitcoinBlockchainInterface::getUnspentTransactions(QList<BitcoinWallet> iWallets, QStringList &oTxids, QStringList &oValues, QStringList &oVouts, int iConfirmations)
{
    QEventLoop              lMyEventLoop;
    QNetworkReply           *lReply;
    bool                    lSuccess = true;

    connect(mNetworkAccessManager, SIGNAL(finished(QNetworkReply*)), &lMyEventLoop, SLOT(quit()));

    for (auto w : mActiveUser->getBrightWallets()) {
        lReply = mNetworkAccessManager->get(QNetworkRequest(QUrl(QString("%1/addrs/%2?unspentOnly=true?confirmations=%3").arg(BLOCKCYPER_API_URL).arg(QString(w.address())).arg(iConfirmations))));
        lMyEventLoop.exec();

        if (lReply->error() == QNetworkReply::NoError) {
            QByteArray      lReplyText = lReply->readAll();
            QJsonDocument   lJsonReponse = QJsonDocument::fromJson(lReplyText);
            QJsonObject     lJsonObject = lJsonReponse.object();
            QVariantMap     lMap = lJsonObject.toVariantMap();

            QList<QVariant> lTxList = lMap["txrefs"].toList();
            oTxids.clear();
            oValues.clear();
            oVouts.clear();

            for (auto w : lTxList) {
                auto lTempMap = w.toMap();
                oTxids.append(lTempMap["tx_hash"].toString());
                oValues.append(lTempMap["value"].toString());
                oVouts.append(lTempMap["tx_output_n"].toString());
            }
            lSuccess = true;
        } else {
            qDebug() << "Error getting unspent transactions!";
            lSuccess = false;
        }
    }

    return lSuccess;
}

void BitcoinBlockchainInterface::createBitcoinTransaction(QList<BitcoinWallet> iInputWallet, QList<BitcoinWallet> iOuputs)
{

}

QString BitcoinBlockchainInterface::estimateMinerFee(int iInputs, int iOutputs)
{
    QEventLoop              lMyEventLoop;
    QNetworkReply           *lReply;
    bool                    lSuccess = true;
    QStringMath             lEstimatedFee = QStringMath();

    connect(mNetworkAccessManager, SIGNAL(finished(QNetworkReply*)), &lMyEventLoop, SLOT(quit()));

    lReply = mNetworkAccessManager->get(QNetworkRequest(QUrl(QString("%1").arg(BLOCKCYPER_API_URL))));
    lMyEventLoop.exec();

    if (lReply->error() == QNetworkReply::NoError) {
        QByteArray      lReplyText = lReply->readAll();
        QJsonDocument   lJsonReponse = QJsonDocument::fromJson(lReplyText);
        QJsonObject     lJsonObject = lJsonReponse.object();
        QVariantMap     lMap = lJsonObject.toVariantMap();

        double lMediumMinerFeesRate = lMap["medium_fee_per_kb"].toDouble();
        double lTransactionSizeEstimateInBytes = ((iInputs * 180) + (iOutputs * 34) + 10 );
        double lTransactionFee = lMediumMinerFeesRate * (lTransactionSizeEstimateInBytes/1000);
        lEstimatedFee = QStringMath::roundUpToNearest0001(QStringMath::btcFromSatoshi(QString::number(lTransactionFee)).toString()).toString();

        qDebug() << "lMediumMinerFeesRate: " << lMediumMinerFeesRate;
        qDebug() << "lTransactionSizeEstimateInBytes: " << lTransactionSizeEstimateInBytes;
        qDebug() << "lTransactionFee: " << lTransactionFee;
        qDebug() << "lEstimatedFee: " << lEstimatedFee.toString();
    } else {
        qDebug() << "Error.... Fee estimation failed";
        lEstimatedFee = QStringMath();
    }

    return lEstimatedFee.toString();
}

void BitcoinBlockchainInterface::replyFinished(QNetworkReply *iReply)
{
    if (iReply->error() == QNetworkReply::NoError) {
        QByteArray lReplyText = iReply->readAll();
        QJsonDocument lJsonReponse = QJsonDocument::fromJson(lReplyText);
        QJsonObject lJsonObject = lJsonReponse.object();

        QVariantMap lMap = lJsonObject.toVariantMap();
        QString lValue = lMap["balance"].toString();
        emit updateWalletBalance(mBitcoinWalletToCheck.walletId(), lValue);
    } else {
        qDebug() << "Error.... ";
        emit updateWalletBalance(mBitcoinWalletToCheck.walletId(), mActiveUser->getBrightBalance().toString());
    }
}
