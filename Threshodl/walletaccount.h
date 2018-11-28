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
    QString owner();

    void setExchangeCurrency(QString iCurrency);
    void setMarketValue(QString iCurrentValue);
    void setConfirmedBalance(QString iBalance);
    void setUnconfirmedBalance(QString iBalance);
    void setOwner(QString iOwner);
    void setPublicKey(QString iPub);

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
    void sortDarkWallets();
    void setDarkWallets(QList<CryptoWallet> wallets);

    void createNewBrightWallet(QString iSeed);

    bool createBrightRawTransaction(QStringList iToAddresses, QStringList iToAmounts, QString &oTxHex, QString &oFee);
    bool createBrightRawTransaction(QString iToAddress, QString iToAmount, QString &oTxHex, QString &oFee);
    bool sendRawTransaction(QString iRawTransaction, QString &oTxid);

    bool estimateMicroWallets(QString iAmount, QString &oAmountWithoutFee, int &oBreaks, QString &oFee, QString &oError);
    bool createMicroWallets(QString iAmount, int &oBreaks, QString &oFinalAmount, QString iPublicKey, QString &oError);
    bool getPendingWalletAddrAndAmounts(QStringList &oAddresses, QStringList &oAmounts);
    void clearPendingWallets();
    bool movePendingToDarkWallet();

    const QByteArray toData();

private:
    QString                 mShortName;
    QString                 mLongName;
    bool                    mIsDark;
    QString                 mOwner;
    QString                 mPublicKey;

    QString                 mConfirmedBalance;
    QString                 mUnconfirmedBalance;

    CryptoNetwork           mChain;

    QString                 mMarketValue;
    QString                 mExchangeCurrency;

    QList<CryptoWallet>     mWallets;
    QList<CryptoWallet>     mPendingWallets;
    MyQSettingsManager      *mAccountData;
};

#endif // WALLETACCOUNT_H
