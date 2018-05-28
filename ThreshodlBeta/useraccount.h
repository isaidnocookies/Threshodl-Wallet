#ifndef USERACCOUNT_H
#define USERACCOUNT_H

#include <QObject>
#include <QMap>
#include <QThread>

#include "myqsettingsmanager.h"
#include "createusername.h"
#include "globalsandconstants.h"
#include "walletaccount.h"
#include "downloadworker.h"

class UserAccount : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString username READ username WRITE setUsername NOTIFY usernameChanged)
    Q_PROPERTY(bool waiting READ waiting WRITE setWaiting NOTIFY waitingChanged)
    Q_PROPERTY(QString currentErrorString READ currentErrorString WRITE setCurrentErrorString NOTIFY currentErrorStringChanged)

public:
    explicit            UserAccount(QObject *parent = nullptr);
    Q_INVOKABLE bool    exists();
    Q_INVOKABLE void    createNewAccount(QString iUsername);
    Q_INVOKABLE QString getTotalBalance(QString iCurrency = "USD");
    Q_INVOKABLE QString getBalance(QString iShortName, bool iIsDark = false, bool iConfirmed = true);
    Q_INVOKABLE QString getBalanceValue(QString iShortName, bool iIsDark = false, bool iConfirmed = true, QString iCurrency = "USD");
    Q_INVOKABLE bool    isWalletConfirmed(QString iCurrency = "BTC", QString iWalletType = "Both");
    Q_INVOKABLE void    setWaiting(bool iWaiting);
    Q_INVOKABLE bool    waiting();
    Q_INVOKABLE QString getBrightAddress(QString iShortname);

    QString username()                      { return mUsername; }
    void setUsername                        (QString iUsername);

    QString currentErrorString()            { return mCurrentErrorString; }
    void setCurrentErrorString              (QString iCurrentErrorString);

    void publicAndPrivateKeys               (QByteArray &oPublicKey, QByteArray &oPrivateKey);
    void setPublicAndPrivateKeys            (QByteArray iPublicKey, QByteArray iPrivateKey);

signals:
    void usernameChanged();
    void waitingChanged();
    void currentErrorStringChanged();

    void cryptoConfirmedBalanceChanged();
    void darkCryptoConfirmedBalanceChanged();
    void darkCryptoUnconfirmedBalanceChanged();

public slots:
    void usernameCreated (bool iSuccess, QString iUsername, QByteArray iPublicKey, QByteArray iPrivateKey);

private:
    void loadAccountFromSettings();
    void createCryptoWallets();

    MyQSettingsManager                *mDataManager;
    CreateUsername                  *mCreateUsername;

    bool                            mWaiting;
    QString                         mCurrentErrorString;
    QString                         mUsername;
    QByteArray                      mPrivateKey;
    QByteArray                      mPublicKey;

    QMap<QString, WalletAccount>    mWallets;


    QThread                         *mMyDownloaderThread;
    DownloadWorker                  *mMyDownloaderWorker;
};

#endif // USERACCOUNT_H
