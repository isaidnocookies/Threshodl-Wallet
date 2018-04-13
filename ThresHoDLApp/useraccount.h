#ifndef USERACCOUNT_H
#define USERACCOUNT_H

#include "bitcoinwallet.h"

#include <QString>
#include <QVector>
#include <QPair>
#include <QDate>
#include <QVariant>
#include <QSettings>
#include <algorithm>

class UserAccount
{
public:
    UserAccount();

    void createNewAccount(QString iUsername, QByteArray iPriv, QByteArray iPub);

    QString                 getUsername();
    QByteArray              getPublicKey();
    QByteArray              getPrivateKey();
    QString                 getEmail();
    QList<QString>          getNotifications();
    QList<BitcoinWallet>    getBrightWallets();
    QList<BitcoinWallet>    getDarkWallets();
    BitcoinWallet           getBrightWallet(int iIndex = 0);
    double                  getDarkBalance()                    { return mDarkBalance; }
    double                  getBrightBalance()                  { return mBrightBalance; }

    void setUsername(QString iUsername);
    void setKeys(QByteArray iPub, QByteArray iPriv);
    void addNotification(QString iTime, QString iNotificationText);
    void setNotifications(QList<QVariant> lNotifications);
    void setAddresses(QString iEmail, QString iThreshodlAddress);

    void createNewBrightWallet();
    void addMicroWallet(BitcoinWallet iWallet);
    void addBrightWallet(BitcoinWallet iWallet);
    void setBrightWallets(QList<BitcoinWallet> iWallets);
    void setDarkWallets(QList<BitcoinWallet> iWallets);
    void setBrightBalance(double iValue)                        { mBrightBalance = iValue; }
    void setDarkBalance(double iValue)                          { mDarkBalance = iValue; }

    bool isNewAccount();

private:
    QSettings                       *mAccountSettings;
    QString                         mUsername;
    QByteArray                      mPublicKey;
    QByteArray                      mPrivateKey;
    QList<QString>                  mNotifications;
    QString                         mEmail;

    QList<BitcoinWallet>            mBrightWallet;
    double                          mBrightBalance;
    QList<BitcoinWallet>            mDarkWallet;
    double                          mDarkBalance;

    void loadFromSettings();
    bool greaterThanWallet (BitcoinWallet iLHS, BitcoinWallet iRHS);
};

#endif // USERACCOUNT_H
