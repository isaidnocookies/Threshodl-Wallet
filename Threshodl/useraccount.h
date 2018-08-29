#ifndef USERACCOUNT_H
#define USERACCOUNT_H

#include <QObject>
#include <QMap>
#include <QThread>
#include <QWaitCondition>

#include "core.h"
#include "myqsettingsmanager.h"
#include "createusername.h"
#include "walletaccount.h"
#include "downloadworker.h"

class UserAccount : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString username READ username WRITE setUsername NOTIFY usernameChanged)
    Q_PROPERTY(QString recoverySeed READ recoverySeed WRITE setRecoverySeed NOTIFY recoverySeedChanged)

public:
    explicit            UserAccount(QObject *parent = nullptr);
    ~UserAccount();
    Q_INVOKABLE bool    exists();
    Q_INVOKABLE void    createNewAccount(QString iUsername);
    Q_INVOKABLE void    recoverAccount(QString iSeed);
    Q_INVOKABLE QString getRecoverySeed();

    Q_INVOKABLE bool    checkPasscode(QString iPass);
    Q_INVOKABLE void    changePasscode(QString iNewPass);
    Q_INVOKABLE QString getTempPasscode();
    Q_INVOKABLE void    confirmPasscodeChange();

    Q_INVOKABLE QString getTotalBalance(QString iCurrency = "USD");
    Q_INVOKABLE QString getBalance(QString iShortName, bool iConfirmed = true);
    Q_INVOKABLE QString getBalanceValue(QString iShortName, bool iConfirmed = true, QString iCurrency = "USD");
    Q_INVOKABLE bool    isWalletConfirmed(QString iShortname = "BTC", QString iWalletType = "Both");
    Q_INVOKABLE QString getBrightAddress(QString iShortname);
    Q_INVOKABLE QString getMarketValue(QString iShortname, QString iCurrency = "USD");

    Q_INVOKABLE QString getTotalWalletBalanceValue(QString iShortname, bool iConfirmed = true, QString iCurrency = "USD");

    Q_INVOKABLE void createRawTransaction(QString iShortname, QString toAddress, QString toAmount);
    Q_INVOKABLE void sendRawTransaction(QString iShortname, QString iRawTransaction);
    Q_INVOKABLE QString sendBrightTransaction(QString iShortname, QString toAddress, QString toAmount);
    Q_INVOKABLE QString createBrightWallet(QString iShortname);

    Q_INVOKABLE QVariantList getAllWallets();

    QString username()                      { return mUsername; }
    void setUsername                        (QString iUsername);

    QString recoverySeed()                  { return mRecoverySeed; }
    void setRecoverySeed                    (QString iSeed);

    void publicAndPrivateKeys               (QString &oPublicKey, QString &oPrivateKey);
    void setPublicAndPrivateKeys            (QString iPublicKey, QString iPrivateKey);
    void setPasscode                        (QString iPasscode);

signals:
    void usernameChanged();
    void recoverySeedChanged();
    void marketValueChanged();
    void walletBalanceUpdateComplete(QString shortname);
    void darkCryptoConfirmedBalanceChanged();
    void darkCryptoUnconfirmedBalanceChanged();
    void setDownloaderAddresses(QString iShortName, QStringList iAddresses);
    void userCreationFinished(bool success, QString error);
    void rawTransactionCreated(bool success, QString lHex, QString lFee);
    void rawTransactionSent(bool success, QString lTxid);

public slots:
    void usernameCreated (bool iSuccess, QString iUsername, QString iRecoverySeed, QString iPublicKey, QString iPrivateKey);
    void marketValuesUpdated(QStringList iNames, QStringList iValues);
    void walletBalancesUpdated(QString iShortname, QStringList iAddresses, QStringList iBalances, QStringList iPendingBalances);
    void downloaderErrorString(QString iError);

private:
    void loadAccountFromSettings();
    void createCryptoWallets();

    MyQSettingsManager *mDataManager;
    CreateUsername *mCreateUsername;

    bool mWaiting;
    QString mCurrentErrorString;
    QString mUsername;
    QString mRecoverySeed;
    QString mPrivateKey;
    QString mPublicKey;

    QMap<QString, WalletAccount> mBrightWallets;

    QWaitCondition *mDownloaderWaitCondition;
    QMutex *mDownloaderMutex;

    QThread *mMyDownloaderThread;
    DownloadWorker *mMyDownloaderWorker;

    QString mPasscode;
    QString mTempPasscode;
};

#endif // USERACCOUNT_H
