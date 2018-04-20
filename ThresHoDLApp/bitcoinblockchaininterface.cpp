#include "bitcoinblockchaininterface.h"

extern "C" {
#include <btc/chainparams.h>
#include <btc/ecc.h>
#include <btc/tool.h>
#include <btc/utils.h>
}

#include <QEventLoop>

BitcoinBlockchainInterface::BitcoinBlockchainInterface(QObject *parent) : QObject(parent)
{
}

void BitcoinBlockchainInterface::setActiveUser(UserAccount *iActiveUser)
{
    mActiveUser = iActiveUser;
}

bool BitcoinBlockchainInterface::updateBrightWalletBalances(int iConfirmations)
{
    QEventLoop      lMyEventLoop;
    QNetworkReply   *lReply;
    QStringMath     lBrightBalance = QStringMath();

    mNetworkAccessManager = new QNetworkAccessManager(this);
    connect(mNetworkAccessManager, SIGNAL(finished(QNetworkReply*)), &lMyEventLoop, SLOT(quit()));

    for (BitcoinWallet &w : mActiveUser->getBrightWallets()) {
        lReply = mNetworkAccessManager->get(QNetworkRequest(QUrl(QString("%1/addrs/%2/balance").arg(BLOCKCYPER_API_URL).arg(QString(w.address())))));
        lMyEventLoop.exec();

        if (lReply->error() == QNetworkReply::NoError) {
            QByteArray lReplyText = lReply->readAll();
            QJsonDocument lJsonReponse = QJsonDocument::fromJson(lReplyText);
            QJsonObject lJsonObject = lJsonReponse.object();
            QVariantMap lMap = lJsonObject.toVariantMap();
            qDebug() << lReplyText;

            QString lBalance = lMap["balance"].toString();

            lBrightBalance = lBrightBalance + lMap["balance"].toString();
        } else {
            qDebug() << "Error.... ";
            mActiveUser->setBrightBalance(lBrightBalance.toString());
            return false;
        }
    }

    mActiveUser->setBrightBalance(lBrightBalance.toString());
    return true;
}

void BitcoinBlockchainInterface::getBalance(QList<BitcoinWallet> iWallet, int iConfirmations)
{
//    mNetworkAccessManager = new QNetworkAccessManager(this);
//    connect (mNetworkAccessManager, &QNetworkAccessManager::finished, this, &BitcoinBlockchainInterface::replyFinished);

//    mBitcoinWalletToCheck = iWallet;

//    mNetworkAccessManager->get(QNetworkRequest(QUrl(QString("%1/addrs/%2").arg(BLOCKCYPER_API_URL).arg(QString(iWallet.address())))));
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
