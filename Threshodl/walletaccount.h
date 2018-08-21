#ifndef WALLETACCOUNT_H
#define WALLETACCOUNT_H

#include <QObject>

#include "core.h"
#include "cryptowallet.h"
#include "myqsettingsmanager.h"

class MyQSettingsManager;

class WalletAccount
{
public:
    WalletAccount();
    WalletAccount(QString iShortName, QString iLongName, CryptoNetwork iChain);
    WalletAccount(QByteArray iData);

    void setDataManager(MyQSettingsManager *iDataMananger);

    QString name();
    QString shortName();
    QString longName();
    QString seed();

    void setExchangeCurrency(QString iCurrency);
    void setMarketValue(QString iCurrentValue);
    void setConfirmedBalance(QString iBalance);
    void setUnconfirmedBalance(QString iBalance);
    void updateMarketValue();

    QString getBalance(bool lIsConfirmed = true);

    QString marketValue();
    QString exchangeCurrency();

    void updateBalance(QString iAddress, QString iBalance, QString iUnconfirmedBalance);

    void addWallet(CryptoWallet iWallet);

    bool removeWallet(QString iAddress);
    bool getWallet(QByteArray &oWallet, QString iAddress);
    bool getWallet(CryptoWallet &oWallet, QString iAddress);
    QStringList getWalletAddresses();
    bool getBrightAddress(QString &oAddress);
    bool hasBrightWallet();

    QList<CryptoWallet> getWallets();

    void createNewBrightWallet(QString iSeed);

    bool createRawTransaction(QString iToAddress, QString iToAmount, QString &oTxHex, QString &oFee);
    bool sendRawTransaction(QString iRawTransaction, QString &oTxid);
    QString sendBrightTransaction(QString iToAddress, QString iToAmount);

    const QByteArray toData();

private:
    QString                 mShortName;
    QString                 mLongName;
    bool                    mIsDark;

    QString                 mConfirmedBalance;
    QString                 mUnconfirmedBalance;

    CryptoNetwork           mChain;

    QString                 mMarketValue;
    QString                 mExchangeCurrency;

    QList<CryptoWallet>     mWallets;
    MyQSettingsManager      *mAccountData;
};

#endif // WALLETACCOUNT_H
