#ifndef USERACCOUNT_H
#define USERACCOUNT_H

#include <QObject>

#include "qsettingsmanager.h"
#include "createusername.h"
#include "globalsandconstants.h"

class UserAccount : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString username READ username WRITE setUsername NOTIFY usernameChanged)
    Q_PROPERTY(bool waiting READ waiting WRITE setWaiting NOTIFY waitingChanged)
    Q_PROPERTY(QString currentErrorString READ currentErrorString WRITE setCurrentErrorString NOTIFY currentErrorStringChanged)

    // Bitcoin
    Q_PROPERTY(QString bitcoinConfirmedBalance READ bitcoinConfirmedBalance WRITE setBitcoinConfirmedBalance NOTIFY bitcoinConfirmedBalanceChanged)
    Q_PROPERTY(QString bitcoinUnconfirmedBalance READ bitcoinUnconfirmedBalance WRITE setBitcoinUnconfirmedBalance NOTIFY bitcoinUnconfirmedBalanceChanged)

    // Ethereum
    Q_PROPERTY(QString ethereumConfirmedBalance READ ethereumConfirmedBalance WRITE setEthereumConfirmedBalance NOTIFY ethereumConfirmedBalanceChanged)
    Q_PROPERTY(QString ethereumUnconfirmedBalance READ ethereumUnconfirmedBalance WRITE setEthereumUnconfirmedBalance NOTIFY ethereumUnconfirmedBalanceChanged)

    // Litecoin
    Q_PROPERTY(QString litecoinConfirmedBalance READ litecoinConfirmedBalance WRITE setLitecoinConfirmedBalance NOTIFY litecoinConfirmedBalanceChanged)
    Q_PROPERTY(QString litecoinUnconfirmedBalance READ litecoinUnconfirmedBalance WRITE setLitecoinUnconfirmedBalance NOTIFY litecoinUnconfirmedBalanceChanged)

public:
    explicit UserAccount(QObject *parent = nullptr);
    Q_INVOKABLE bool exists();
    Q_INVOKABLE void createNewAccount(QString iUsername);
    Q_INVOKABLE QString getTotalBalance(QString iCurrency = "USD");
    Q_INVOKABLE QString getBitcoinBalance(QString iType = "Bright", bool iConfirmed = true);
    Q_INVOKABLE bool isWalletConfirmed(QString iCurrency = "BTC", QString iWalletType = "Both");

    QString username()                      { return mUsername; }
    void setUsername                        (QString iUsername);

    bool waiting()                          { return mWaiting; }
    Q_INVOKABLE void setWaiting             (bool iWaiting);

    QString currentErrorString()            { return mCurrentErrorString; }
    void setCurrentErrorString              (QString iCurrentErrorString);

    void publicAndPrivateKeys               (QByteArray &oPublicKey, QByteArray &oPrivateKey);
    void setPublicAndPrivateKeys            (QByteArray iPublicKey, QByteArray iPrivateKey);

    QString bitcoinConfirmedBalance()       { return mBtcConfirmedBalance; }
    QString bitcoinUnconfirmedBalance()     { return mBtcUnconfirmedBalance; }

    QString ethereumConfirmedBalance()      { return mEthConfirmedBalance; }
    QString ethereumUnconfirmedBalance()    { return mEthUnconfirmedBalance; }

    QString litecoinConfirmedBalance()      { return mLtcConfirmedBalance; }
    QString litecoinUnconfirmedBalance()    { return mLtcUnconfirmedBalance; }

    Q_INVOKABLE void setBitcoinConfirmedBalance         (QString iBtcConfirmed);
    void setBitcoinUnconfirmedBalance       (QString iBtcUnconfirmed);

    void setEthereumConfirmedBalance        (QString iEthConfirmed);
    void setEthereumUnconfirmedBalance      (QString iEthUnconfirmed);

    void setLitecoinConfirmedBalance        (QString iLtcConfirmed);
    void setLitecoinUnconfirmedBalance      (QString iLtcUnconfirmed);

signals:
    void usernameChanged();
    void waitingChanged();
    void currentErrorStringChanged();

    void bitcoinConfirmedBalanceChanged();
    void bitcoinUnconfirmedBalanceChanged();
    void ethereumConfirmedBalanceChanged();
    void ethereumUnconfirmedBalanceChanged();
    void litecoinConfirmedBalanceChanged();
    void litecoinUnconfirmedBalanceChanged();

    void cryptoConfirmedBalanceChanged();
    void darkCryptoConfirmedBalanceChanged();
    void darkCryptoUnconfirmedBalanceChanged();

public slots:
    void usernameCreated (bool iSuccess, QString iUsername, QByteArray iPublicKey, QByteArray iPrivateKey);

private:
    void loadAccountFromSettings();

    QSettingsManager    *mDataManager;
    CreateUsername      *mCreateUsername;

    bool                mWaiting;
    QString             mCurrentErrorString;
    QString             mUsername;
    QByteArray          mPrivateKey;
    QByteArray          mPublicKey;

    QString             mBtcConfirmedBalance;
    QString             mBtcUnconfirmedBalance;

    QString             mBrightBitcoinConfirmedBalance;
    QString             mBrightBitcoinUnconfirmedBalance;
    QString             mDarkBitcoinConfirmedBalance;
    QString             mDarkBitcoinUnconfirmedBalance;

    QString             mEthConfirmedBalance;
    QString             mEthUnconfirmedBalance;

    QString             mLtcConfirmedBalance;
    QString             mLtcUnconfirmedBalance;
};

#endif // USERACCOUNT_H
