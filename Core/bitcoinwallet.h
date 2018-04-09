#ifndef BITCOINWALLET_H
#define BITCOINWALLET_H

#include "wallet.h"
#include <QSharedPointer>

class BitcoinWallet : public Wallet
{
protected:
    BitcoinWallet();

public:
    BitcoinWallet(const QByteArray iData);

    virtual ~BitcoinWallet();

    enum ChainType {
        None            = 0x0,  /* invalid wallet */
        Main            = 0x1,
        TestNet         = 0x2,
        RegressionNet   = 0x3
    };

    typedef enum ChainType ChainType;

    virtual ChainType   chain() const;
    virtual QByteArray  wif() const;
    virtual QByteArray  p2pkhAddress() const;           // Same as address() - Default / Classic BTC address
    virtual QByteArray  p2wpkhAddress();                // Generates and caches a P2KPKH address, does not retain it between object destructions
    virtual QByteArray  p2sh_p2wpkhAddress();           // Generates and caches a P2SH+P2KPKH (aka SEGWIT) address, does not retain it between object destructions

    virtual QByteArray  toData() const override;

    static QSharedPointer<BitcoinWallet> createNewBitcoinWallet(ChainType iChainType = ChainType::Main);
    static QSharedPointer<BitcoinWallet> fromWifAndPrivateKey(const QByteArray iWif, const QByteArray iPrivateKey, ChainType iChainType = ChainType::Main);

protected:
    QVariantMap _bitcoinWalletClassToData(QVariantMap iMap) const;
    void _bitcoinWalletClassFromData(QVariantMap iDataMap);
    void _calculateAddresses();

    ChainType       mChain              = ChainType::None;
    QByteArray      mWif;
    QByteArray      mP2WPKHAddress;         // Cached
    QByteArray      mP2SH_P2WPKHAddress;    // Cached
};

#endif // BITCOINWALLET_H
