#ifndef GENERICWALLET_H
#define GENERICWALLET_H

#include "wallet.h"
#include "globalsandconstants.h"

#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QObject>

class GenericWallet : public Wallet
{
public:
    GenericWallet();
    GenericWallet(const QByteArray iData);
    GenericWallet(const Wallet &iOther);
    GenericWallet(const WalletDataCore &iOther);

    enum ChainType {
        None            = 0x0,  /* invalid wallet */
        Main            = 0x1,
        TestNet         = 0x2,
        RegressionNet   = 0x3
    };

    virtual ~GenericWallet() { }

    virtual QByteArray encodedPrivateKey() const                { return WalletDataCoreValueToByteArray(walletDataCore(),QStringLiteral("genericEncodedPrivateKey")); }
    virtual QString shortName() const                           { return WalletDataCoreValueToString (walletDataCore(),QStringLiteral("genericShortName")); }
    virtual QString name() const                                { return WalletDataCoreValueToString (walletDataCore(),QStringLiteral("genericName")); }
    virtual QString longName() const                            { return WalletDataCoreValueToString (walletDataCore(),QStringLiteral("genericLongName")); }
    virtual QString balance() const                             { return WalletDataCoreValueToString (walletDataCore(),QStringLiteral("genericBalance")); }
    virtual QString unconfirmedBalance() const                  { return WalletDataCoreValueToString (walletDataCore(),QStringLiteral("genericUnconfirmedBalance")); }

    virtual void setEncodedPrivateKey(const QByteArray iValue)  { WalletDataCoreValueFromByteArray(walletDataCore(),QStringLiteral("genericEncodedPrivateKey"),iValue); }
    virtual void setShortName(const QString iValue)             { WalletDataCoreValueFromString(walletDataCore(),QStringLiteral("genericShortName"),iValue); }
    virtual void setName(const QString iValue)                  { WalletDataCoreValueFromString(walletDataCore(),QStringLiteral("genericName"),iValue); }
    virtual void setLongName(const QString iValue)              { WalletDataCoreValueFromString(walletDataCore(),QStringLiteral("genericLongName"),iValue); }
    virtual void setBalance(const QString iValue)               { WalletDataCoreValueFromString(walletDataCore(),QStringLiteral("genericBalance"),iValue); }
    virtual void setUnconfirmedBalance(const QString iValue)    { WalletDataCoreValueFromString(walletDataCore(),QStringLiteral("genericUnconfirmedBalance"),iValue); }

    static GenericWallet createWallet(QString iShortname, QString iSeed, ChainType iChainType = ChainType::TestNet);

private:
    static GenericWallet createDashWallet(QString iSeed, ChainType iChainType = ChainType::TestNet);
    static GenericWallet createLitecoinWallet(QString iSeed, ChainType iChainType = ChainType::TestNet);
    static GenericWallet createZCashWallet(QString iSeed, ChainType iChainType = ChainType::TestNet);
    static GenericWallet createEthWallet(QString iSeed, ChainType iChainType = ChainType::TestNet);
    static GenericWallet createBitcoinWallet(QString iSeed, ChainType iChainType = ChainType::TestNet);
};

#endif // GENERICWALLET_H
