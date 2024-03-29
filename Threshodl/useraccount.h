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
    explicit                   UserAccount(QObject *parent = nullptr);
    ~UserAccount();
    Q_INVOKABLE bool            exists();
    Q_INVOKABLE void            createNewAccount(QString iUsername);
    Q_INVOKABLE void            recoverAccount(QString iSeed);
    Q_INVOKABLE QString         getRecoverySeed();
    Q_INVOKABLE void            changeUsername(QString iUsername);

    Q_INVOKABLE bool            checkPasscode(QString iPass);
    Q_INVOKABLE void            changePasscode(QString iNewPass);
    Q_INVOKABLE QString         getTempPasscode();
    Q_INVOKABLE void            confirmPasscodeChange();
    Q_INVOKABLE QString         hashValue(QString iValue);

    Q_INVOKABLE QString         getTotalBalance(QString iCurrency = "USD");
    Q_INVOKABLE QString         getBalance(QString iShortName, bool iConfirmed = true);
    Q_INVOKABLE QString         getBalanceValue(QString iShortName, bool iConfirmed = true, QString iCurrency = "USD");
    Q_INVOKABLE bool            isWalletConfirmed(QString iShortname = "BTC", QString iWalletType = "Both");
    Q_INVOKABLE QString         getBrightAddress(QString iShortname);
    Q_INVOKABLE QString         getMarketValue(QString iShortname, QString iCurrency = "USD");

    Q_INVOKABLE QString         getEmailAddress();
    Q_INVOKABLE void            setEmailAddress(QString iEmail);

    Q_INVOKABLE QString         getTotalWalletBalanceValue(QString iShortname, bool iConfirmed = true, QString iCurrency = "USD");

    Q_INVOKABLE void            createRawTransaction(QString iShortname, QString toAddress, QString toAmount);
    Q_INVOKABLE void            sendRawTransaction(QString iShortname, QString iRawTransaction);
    Q_INVOKABLE QString         createBrightWallet(QString iShortname);
    Q_INVOKABLE QVariantList    getAllWallets();

    Q_INVOKABLE void            startDarkDeposit(QString iShortname, QString iAmount);
    Q_INVOKABLE void            depositDarkCoin(QString iShortname, QString iAmount, QString iFee);
    Q_INVOKABLE void            withdrawDarkCoin(QString iShortname, QString iAmount, QString iFee);
    Q_INVOKABLE void            estimateDarkWithdrawal(QString iShortname, QString iAmount);
    Q_INVOKABLE void            estimateDarkTransaction(QString iShortname, QString toAmount);
    Q_INVOKABLE void            checkIfDarkTransactionIsPossible(QString iShortname, QString sendAmount);
    Q_INVOKABLE void            sendDarkTransaction(QString iAmount, QString iShortname, QString toAddress, QString toEmail);
    Q_INVOKABLE QVariantList    getDarkWallets(QString iShortname);

    Q_INVOKABLE void            handleFileUrlReceived(const QUrl &url);
    Q_INVOKABLE bool            importWallets();

    QString     username() { return mUsername; }
    void        setUsername (QString iUsername);
    QString     emailAddress() { return mEmailAddress; }
    QString     recoverySeed() { return mRecoverySeed; }
    void        setRecoverySeed (QString iSeed);
    void        publicAndPrivateKeys (QString &oPublicKey, QString &oPrivateKey);
    void        setPublicAndPrivateKeys (QString iPublicKey, QString iPrivateKey);
    void        setPasscode (QString iPasscode);

signals:
    void usernameChanged();
    void usernameChangeSuccess(bool success);
    void recoverySeedChanged();
    void marketValueChanged(QString shortname);
    void walletBalanceUpdateComplete(QString shortname);
    void darkCryptoConfirmedBalanceChanged();
    void darkCryptoUnconfirmedBalanceChanged();
    void setDownloaderAddresses(QString iShortName, QStringList iAddresses);
    void userCreationFinished(bool success, QString error);
    void rawTransactionCreated(bool success, QString lHex, QString lFee);
    void rawTransactionSent(bool success, QString lTxid);

    void darkTransactionEstimated(bool success, QString fee);
    void darkWithdrawalEstimated(bool success, QString fee);
    void darkWithdrawComplete(bool success, int error);
    void darkTransactionSent(bool success, int error);
    void darkDepositConfirmation(bool oSuccess, QString oFee, QString oActualAmountWithoutFee, QString oShortname);
    void darkDepositComplete(bool oSuccess, QString oActualAmountWithoutFees, int oBreaks);
    void darkTransactionIsPossible(bool success, int error);

    void importDarkWalletsSignal(QString type, QString amount, QString notes);

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
    QString mEmailAddress;
    QString mRecoverySeed;
    QString mPrivateKey;
    QString mPublicKey;

    QMap<QString, WalletAccount> mBrightWallets;
    QMap<QString, WalletAccount> mDarkWallets;

    QWaitCondition *mDownloaderWaitCondition;
    QMutex *mDownloaderMutex;

    QThread *mMyDownloaderThread;
    DownloadWorker *mMyDownloaderWorker;

    QString mPasscode;
    QString mTempPasscode;

    QByteArray mWalletDataToImport;
};

#endif // USERACCOUNT_H
