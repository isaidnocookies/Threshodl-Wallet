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
#include <QQueue>

class UserAccount;

class BitcoinBlockchainInterface : public QObject
{
    Q_OBJECT
public:
    explicit BitcoinBlockchainInterface(QObject *parent = nullptr);
    void setActiveUser (UserAccount *iActiveUser);

    bool updateBrightWalletBalances(int iConfirmations = 1);
    bool getUnspentTransactions(QList<BitcoinWallet> iWallets, QStringList &oTxids, QStringList &oValues, QStringList &oVouts, int iConfirmations = 1);
    void createBitcoinTransaction(QList<BitcoinWallet> iInputWallet, QList<BitcoinWallet> iOuputs);
    QString estimateMinerFee(int iInputs, int iOutputs);

signals:
    void updateWalletBalance (QString iWalletID, QString iBalance);
    void brightWalletUpdateComplete (bool iSuccess);

private slots:
    void replyFinished(QNetworkReply *iReply);

private:
    QNetworkAccessManager   *mNetworkAccessManager;
    QNetworkReply           *mNetworkReply;
    UserAccount             *mActiveUser;
    BitcoinWallet           mBitcoinWalletToCheck;
    QQueue<BitcoinWallet>   mWalletsToUpdate;
};

#endif // BITCOINBLOCKCHAININTERFACE_H
