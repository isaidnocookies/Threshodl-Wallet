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
    bool getUnspentTransactions(QList<BitcoinWallet> iWallets, QStringList &oTxids, QStringList &oValues, QStringList &oVouts, QList<QByteArray> &oPrivateKeys, int iConfirmations = 1);
    QByteArray createBitcoinTransaction(QList<BitcoinWallet> iInputWallets, QList<BitcoinWallet> iOutputs, QString iMinerFee, QList<QByteArray> &oPrivateKeys);
    QString estimateMinerFee(int iInputs, int iOutputs, bool iRoundUp = true, int iBlocksToBeConfirmed = 3);
    bool signRawTransaction(QByteArray iRawTransaction, QByteArray &oSignedHex, QList<QByteArray> iPrivateKeys);
    bool sendRawTransaction(QByteArray iSignedHex);

signals:
    void updateWalletBalance (QString iWalletID, QString iBalance);
    void brightWalletUpdateComplete (bool iSuccess);

private:
    QNetworkAccessManager   *mNetworkAccessManager;
    QNetworkReply           *mNetworkReply;
    UserAccount             *mActiveUser;
    BitcoinWallet           mBitcoinWalletToCheck;
    QQueue<BitcoinWallet>   mWalletsToUpdate;
};

#endif // BITCOINBLOCKCHAININTERFACE_H
