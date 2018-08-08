#ifndef WALLETACCOUNT_H
#define WALLETACCOUNT_H

#include <QObject>

#include "core.h"
#include "genericwallet.h"
#include "globalsandconstants.h"
#include "myqsettingsmanager.h"

class MyQSettingsManager;

class WalletAccount
{
public:
    WalletAccount();
    WalletAccount(QString iShortName, QString iLongName, QString iSeed, CryptoChain iChain);
    WalletAccount(QByteArray iData);

    void setDataManager(MyQSettingsManager *iDataMananger);

    QString name();
    QString shortName();
    QString longName();
    QString seed();

    void setExchangeCurrency(QString iCurrency);
    void setMarketValue(QString iCurrentValue);
    void setConfirmedBalance(QString iBalance);
    void updateMarketValue();

    QString getBalance(bool lIsConfirmed = true);

    QString marketValue();
    QString exchangeCurrency();

    void updateBalance(QString iAddress, QString iBalance, QString iUnconfirmedBalance);

    void addWallet(Wallet iWallet);

    bool removeWallet(QString iAddress);
    bool getWallet(QByteArray &oWallet, QString iAddress);
    bool getWallet(Wallet &oWallet, QString iAddress);
    QStringList getWalletAddresses();
    bool getBrightAddress(QString &oAddress);

    void createNewBrightWallet();

    const QByteArray toData();

//signals:
//    void confirmedBalanceChanged();
//    void unconfirmedBalanceChanged();
//    void marketValueChanged();

private:
    QString                 mShortName;
    QString                 mLongName;
    QString                 mName;
    bool                    mIsDark;
    QString                 mSeed;

    QString                 mConfirmedBalance;
    QString                 mUnconfirmedBalance;

    CryptoChain             mChain;

    QString                 mMarketValue;
    QString                 mExchangeCurrency;

    QList<GenericWallet>    mWallets;
    MyQSettingsManager      *mAccountData;
};

#endif // WALLETACCOUNT_H
