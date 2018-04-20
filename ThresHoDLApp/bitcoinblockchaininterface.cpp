#include "bitcoinblockchaininterface.h"

extern "C" {
#include <btc/chainparams.h>
#include <btc/ecc.h>
#include <btc/tool.h>
#include <btc/utils.h>
}

BitcoinBlockchainInterface::BitcoinBlockchainInterface(QObject *parent) : QObject(parent)
{
    mNetworkAccessManager = new QNetworkAccessManager(this);
    connect (mNetworkAccessManager, &QNetworkAccessManager::finished, this, &BitcoinBlockchainInterface::replyFinished);
}

void BitcoinBlockchainInterface::setActiveUser(UserAccount *iActiveUser)
{
    mActiveUser = iActiveUser;
}

QString BitcoinBlockchainInterface::getBalance(BitcoinWallet iWallet, int iConfirmations)
{
    QNetworkRequest mRequest(QUrl(QString("%1/addrs/%2").arg(BLOCKCYPER_API_URL).arg(QString(iWallet.address()))));
    mNetworkAccessManager->get(mRequest);
}

void BitcoinBlockchainInterface::replyFinished(QNetworkReply *reply)
{
    qDebug() << "Reply!";
    qDebug() << reply->readAll();
    emit updateWalletBalance("", "");
}
