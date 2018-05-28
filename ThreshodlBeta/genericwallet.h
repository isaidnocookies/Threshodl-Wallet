#ifndef GENERICWALLET_H
#define GENERICWALLET_H

#include "wallet.h"

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

    static GenericWallet createWallet(QString iShortname, ChainType iChainType = ChainType::TestNet);

    static QByteArray generateBtcWifFromPrivateKey(const QByteArray iPrivateKey, ChainType iChainType = ChainType::Main);
    static GenericWallet fromBtcWifAndPrivateKey(const QByteArray iWif, const QByteArray iPrivateKey, ChainType iChainType = ChainType::Main);

private:
    static GenericWallet createDashWallet(ChainType iChainType = ChainType::TestNet);
    static GenericWallet createLitecoinWallet(ChainType iChainType = ChainType::TestNet);
    static GenericWallet createZCashWallet(ChainType iChainType = ChainType::TestNet);
    static GenericWallet createEthWallet(ChainType iChainType = ChainType::TestNet);
};

#endif // GENERICWALLET_H
