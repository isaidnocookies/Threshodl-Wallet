#ifndef USERACCOUNT_H
#define USERACCOUNT_H

#include "bitcoinwallet.h"
#include "qstringmath.h"
#include "bitcoinblockchaininterface.h"

#include <QString>
#include <QVector>
#include <QPair>
#include <QDate>
#include <QDateTime>
#include <QVariant>
#include <QSettings>
#include <algorithm>
#include <QObject>
#include <QSet>

class BitcoinBlockchainInterface;

class UserAccount : public QObject
{
    Q_OBJECT

public:
    UserAccount(QObject *parent = nullptr);

    void createNewAccount(QString iUsername, QByteArray iPriv, QByteArray iPub);

    QString                 getUsername();
    QByteArray              getPublicKey();
    QByteArray              getPrivateKey();
    QString                 getEmail();
    QList<QString>          getNotifications();
    QList<BitcoinWallet>    getBrightWallets();
    QList<BitcoinWallet>    getDarkWallets();
    BitcoinWallet           getBrightWallet(int iIndex = 0);
    QStringMath             getDarkBalance()                    { return mDarkBalance; }
    QStringMath             getBrightBalance()                  { return mBrightBalance; }
    QList<BitcoinWallet>    getPendingToSendDarkWallets();

    void setUsername(QString iUsername);
    void setKeys(QByteArray iPub, QByteArray iPriv);
    void addNotification(QString iTime, QString iNotificationText);
    void setNotifications(QList<QVariant> lNotifications);
    void setAddresses(QString iEmail, QString iThreshodlAddress);
    void savePendingToSendDarkWallets(QList<BitcoinWallet> iWallets);
    void clearPendingToSendDarkWallets();

    void createNewBrightWallet();
    void addMicroWallet(BitcoinWallet iWallet);
    void addBrightWallet(BitcoinWallet iWallet);
    void removeMicroWallet(BitcoinWallet iWallet);
    void setBrightWallets(QList<BitcoinWallet> iWallets);
    void setDarkWallets(QList<BitcoinWallet> iWallets);
    void setBrightBalance(QStringMath iValue)                        { mBrightBalance = iValue; }
    void setDarkBalance(QStringMath iValue)                          { mDarkBalance = iValue; }
    bool isNewAccount();
    bool accountContainsWallet (QString iWalletId);

    void updateBrightBalanceFromBlockchain();

    void removeBrightWallets(QString iAmount); //for testing...

public slots:
    void updateFromBrightComplete(bool iSuccess);

signals:
    void updateBalancesForMainWindow(QString iBright, QString iDark);
    void updateBrightBalanceComplete(bool oSuccess);

private:
    QSettings                       *mAccountSettings;
    BitcoinBlockchainInterface      *mBitcoinBlockchainInterface;
    QString                         mUsername;
    QByteArray                      mPublicKey;
    QByteArray                      mPrivateKey;
    QList<QString>                  mNotifications;
    QString                         mEmail;

    QList<BitcoinWallet>            mBrightWallet;
    QStringMath                     mBrightBalance;
    QList<BitcoinWallet>            mDarkWallet;
    QStringMath                     mDarkBalance;

    QSet<QString>                   mAllWallets;

    void loadFromSettings();
    bool greaterThanWallet (BitcoinWallet iLHS, BitcoinWallet iRHS);
    void sortWallet (bool iSortDark);
};

#endif // USERACCOUNT_H
