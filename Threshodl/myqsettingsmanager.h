#ifndef QSETTINGSMANAGER_H
#define QSETTINGSMANAGER_H

#include <QObject>
#include <QSettings>

#include "core.h"

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
    void saveWalletAccount(QString iShortName, QString iLongName, CryptoNetwork iChainType);

    void savePasscode(QString iPasscode);
    QString getPasscode();

    void saveEmailAddress(QString iEmail);
    QString getEmail();

    void saveRecoverySeed(QString iSeed);
    QString getRecoverySeed();

    void getBrightWalletAccounts(QList<WalletAccount> &oWalletAccounts);
    void getDarkWalletAccounts(QList<WalletAccount> &oDarkWalletAccounts);

    void getWalletBalance(QString iShortname, QString &oConfirmed, QString &oUnconfirmed);
    void saveWalletBalance(QString iShortname, QString iConfirmedBalance, QString iUnconfirmedBalance);

protected:
    QSettings *mAccountData;
};

#endif // QSETTINGSMANAGER_H
