#ifndef BITCOINWALLET_H
#define BITCOINWALLET_H

#include "wallet.h"
#include <QSharedPointer>

class BitcoinWallet : public Wallet
{
public:
    BitcoinWallet();
    BitcoinWallet(const QByteArray iData);
    BitcoinWallet(const Wallet &iOther);
    BitcoinWallet(const WalletDataCore &iOther);

    virtual ~BitcoinWallet() { }

    enum ChainType {
        None            = 0x0,  /* invalid wallet */
        Main            = 0x1,
        TestNet         = 0x2,
        RegressionNet   = 0x3
    };

    typedef enum ChainType ChainType;

    virtual ChainType   chain() const                                       { return static_cast<ChainType>(WalletDataCoreValueToUInt(walletDataCore(),QStringLiteral("btcChain"))); }
    virtual QByteArray  wif() const                                         { return WalletDataCoreValueToByteArray(walletDataCore(),QStringLiteral("btcWifAddress")); }
    virtual QByteArray  P2PKHAddress() const                                { return address(); }
    virtual QByteArray  P2WPKHAddress() const                               { return WalletDataCoreValueToByteArray(walletDataCore(),QStringLiteral("btcP2WPHKAddress")); }
    virtual QByteArray  P2SH_P2WPKHAddress() const                          { return WalletDataCoreValueToByteArray(walletDataCore(),QStringLiteral("btcP2SH+P2WPHKAddress")); }

    virtual void        setChain(const ChainType iValue)                    { WalletDataCoreValueFromUInt(walletDataCore(),QStringLiteral("btcChain"),static_cast<unsigned int>(iValue)); }
    virtual void        setWif(const QByteArray iValue)                     { WalletDataCoreValueFromByteArray(walletDataCore(),QStringLiteral("btcWifAddress"),iValue); }
    virtual void        setP2PKHAddress(const QByteArray iValue)            { setAddress(iValue); }
    virtual void        setP2WPKHAddress(const QByteArray iValue)           { WalletDataCoreValueFromByteArray(walletDataCore(),QStringLiteral("btcP2WPHKAddress"),iValue); }
    virtual void        setP2SH_P2WPKHAddress(const QByteArray iValue)      { WalletDataCoreValueFromByteArray(walletDataCore(),QStringLiteral("btcP2SH+P2WPHKAddress"),iValue); }

    static BitcoinWallet createNewBitcoinWallet(ChainType iChainType = ChainType::Main);
    static BitcoinWallet fromWifAndPrivateKey(const QByteArray iWif, const QByteArray iPrivateKey, ChainType iChainType = ChainType::Main);
};

#endif // BITCOINWALLET_H
