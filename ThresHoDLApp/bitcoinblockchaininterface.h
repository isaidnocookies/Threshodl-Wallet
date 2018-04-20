#ifndef BITCOINBLOCKCHAININTERFACE_H
#define BITCOINBLOCKCHAININTERFACE_H

#include "globalsandstyle.h"
#include "bitcoinwallet.h"
#include "useraccount.h"

#include <QObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

class BitcoinBlockchainInterface : public QObject
{
    Q_OBJECT
public:
    explicit BitcoinBlockchainInterface(QObject *parent = nullptr);
    void setActiveUser (UserAccount *iActiveUser);

    QString getBalance (BitcoinWallet iWallets, int iConfirmations = 1);
    void getUnspentTransactions (QList<BitcoinWallet> iWallets, int iConfirmations = 1);

signals:
    void updateWalletBalance (QString iWalletID, QString iBalance);

private slots:
    void replyFinished(QNetworkReply *reply);

private:
    QNetworkAccessManager   *mNetworkAccessManager;
    QNetworkReply           *mNetworkReply;
    UserAccount             *mActiveUser;
};

#endif // BITCOINBLOCKCHAININTERFACE_H
