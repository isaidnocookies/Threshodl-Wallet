#ifndef QSETTINGSMANAGER_H
#define QSETTINGSMANAGER_H

#include <QObject>
#include <QSettings>

#include "core.h"
#include "cryptowallet.h"

class WalletAccount;

class MyQSettingsManager : public QObject
{
    Q_OBJECT
public:
    explicit MyQSettingsManager(QObject *parent = nullptr);

    bool userAccountExists();
    void saveUsername(QString iUsername);
    void savePublicAndPrivateKeys(QString iPublicKey, QString iPrivateKey);
    void usernameAndKeys(QString &oUsername, QString &oPublicKey, QString &oPrivateKey);

    void saveWallet(QByteArray iWalletData, QString iShortname, bool isDark);
    void saveWallets(QList<QByteArray> iWallets, QString iShortname, bool isDark);
    void removeWallet(QString iShortname, QString iAddress, bool isDark);
    void saveWalletAccount(QString iShortName, QString iLongName, CryptoNetwork iChainType);

    void savePendingMicroWallet(QByteArray iWalletData, QString iShortname);
    void getPendingMicroWallets(QString iShortname, QList<CryptoWallet> &oPendingWallets);
    void clearPendingMicroWallet(QString iShortname, QStringList iAddresses);
    void clearAllPendingMicroWallets(QString iShortname);

    void saveWalletToSweep(QByteArray iWalletData, QString iShortname);
    void getWalletsToSweep(QString iShortname, QList<CryptoWallet> oWalletsToSweep);
    void clearWalletsToSweep(QString iShortname);

    void savePasscode(QString iPasscode);
    QString getPasscode();

    void saveEmailAddress(QString iEmail);
    QString getEmail();

    void saveRecoverySeed(QString iSeed);
    QString getRecoverySeed();

    void saveMarketValue(QString iShortname, QString iMarketValue);
    QString getMarketValue(QString iShortname);

    void getBrightWalletAccounts(QList<WalletAccount> &oWalletAccounts);
    void getDarkWalletAccounts(QList<WalletAccount> &oDarkWalletAccounts);

    void getWalletBalance(QString iShortname, QString &oConfirmed, QString &oUnconfirmed);
    void getDarkWalletBalance(QString iShortname, QString &oConfirmed, QString &oUnconfirmed);
    void saveWalletBalance(QString iShortname, QString iConfirmedBalance, QString iUnconfirmedBalance);

protected:
    QSettings *mAccountData;
};

#endif // QSETTINGSMANAGER_H
