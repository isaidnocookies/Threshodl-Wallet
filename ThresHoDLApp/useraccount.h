#ifndef USERACCOUNT_H
#define USERACCOUNT_H

#include "bitcoinwallet.h"

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
    double                  getDarkBalance()                    { return mDarkBalance; }
    double                  getBrightBalance()                  { return mBrightBalance; }
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
    void setBrightWallets(QList<BitcoinWallet> iWallets);
    void setDarkWallets(QList<BitcoinWallet> iWallets);
    void setBrightBalance(double iValue)                        { mBrightBalance = iValue; }
    void setDarkBalance(double iValue)                          { mDarkBalance = iValue; }

    bool isNewAccount();
    bool accountContainsWallet (QString iWalletId);

signals:
    void updateBalancesForMainWindow(double iBright, double iDark);

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

    QSet<QString>                   mAllWallets;

    void loadFromSettings();
    bool greaterThanWallet (BitcoinWallet iLHS, BitcoinWallet iRHS);
};

#endif // USERACCOUNT_H
