#ifndef QSETTINGSMANAGER_H
#define QSETTINGSMANAGER_H

#include <QObject>
#include <QSettings>

#include "globalsandconstants.h"
#include "walletaccount.h"

class WalletAccount;

class MyQSettingsManager : public QObject
{
    Q_OBJECT
public:
    explicit MyQSettingsManager(QObject *parent = nullptr);

    bool userAccountExists();
    void saveUsername(QString iUsername);
    void savePublicAndPrivateKeys(QByteArray iPublicKey, QByteArray iPrivateKey);
    void usernameAndKeys(QString &oUsername, QByteArray &oPublicKey, QByteArray &oPrivateKey);

    void saveWallet(QByteArray iWalletData, QString iShortname, bool isDark);
    void saveWalletAccount(QString iShortName, QString iLongName, CryptoChain iChainType);

    void getWalletAccounts(QList<WalletAccount> &oWalletAccounts);

protected:
    QSettings *mAccountData;
};

#endif // QSETTINGSMANAGER_H
