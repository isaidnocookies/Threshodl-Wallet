#include "bitcoinwallet.h"

#include <QMutex>

extern "C" {
#include <btc/chainparams.h>
#include <btc/ecc.h>
#include <btc/tool.h>
}

#define     kBTCStringBufferSize    128

static QMutex       sInitLock;
static bool         sInitDone   = false;

inline static void __init() {
    QMutexLocker l{&sInitLock};
    if( ! sInitDone ) {
        btc_ecc_start();
        sInitDone = true;
    }
}

BitcoinWallet::BitcoinWallet() : Wallet()
{ __init(); }

BitcoinWallet::BitcoinWallet(const QByteArray iData) : Wallet(iData)
{ __init(); }

BitcoinWallet::BitcoinWallet(const Wallet &iOther) : Wallet(iOther)
{ __init(); }

BitcoinWallet::BitcoinWallet(const WalletDataCore &iOther) : Wallet(iOther)
{ __init(); }

BitcoinWallet BitcoinWallet::createNewBitcoinWallet(ChainType iChainType)
{
    __init();

    const btc_chainparams *     lChain              = &btc_chainparams_main;

    switch( iChainType ) {
    case ChainType::TestNet:
        lChain = &btc_chainparams_test;
        break;
    case ChainType::RegressionNet:
        lChain = &btc_chainparams_regtest;
        break;
    default:
        // Default to main, do nothing
        break;
    }

    QByteArray                  lWif{128,0};
    QByteArray                  lPrivateKey{128,0};
    if( ! gen_privatekey(lChain, lWif.data(), lWif.size(), lPrivateKey.data() ) )
        return BitcoinWallet{};

    lWif.resize(qstrlen(lWif.constData())+1);
    lPrivateKey.resize(qstrlen(lPrivateKey.constData())+1);
    return fromWifAndPrivateKey(lWif,lPrivateKey,iChainType);
}

BitcoinWallet BitcoinWallet::fromWifAndPrivateKey(const QByteArray iWif, const QByteArray iPrivateKey, ChainType iChainType)
{
    __init();

    const btc_chainparams *     lChain              = &btc_chainparams_main;

    switch( iChainType ) {
    case ChainType::TestNet:
        lChain = &btc_chainparams_test;
        break;
    case ChainType::RegressionNet:
        lChain = &btc_chainparams_regtest;
        break;
    default:
        // Default to main, do nothing
        break;
    }

    // Get the public key first
    QByteArray  lPublicKey{kBTCStringBufferSize,0};
    size_t      lPublicSize = kBTCStringBufferSize;

    if( ! pubkey_from_privatekey(lChain, iWif.constData(), lPublicKey.data(), &lPublicSize) )
        return BitcoinWallet{};

    lPublicKey.resize(lPublicSize);

    // Now Generate addresses
    QByteArray                  lP2PKH{kBTCStringBufferSize,0};
    QByteArray                  lP2WPKH{kBTCStringBufferSize,0};
    QByteArray                  lP2SH_P2WPKH{kBTCStringBufferSize,0};

    if( ! addresses_from_pubkey(lChain, lPublicKey.constData(), lP2PKH.data(), lP2SH_P2WPKH.data(), lP2WPKH.data()) )
        return BitcoinWallet{};

    lP2PKH.resize(qstrlen(lP2PKH.constData()) + 1);
    lP2WPKH.resize(qstrlen(lP2WPKH.constData()) + 1);
    lP2SH_P2WPKH.resize(qstrlen(lP2SH_P2WPKH.constData()) + 1);

    BitcoinWallet lWallet;

    lWallet.setChain(iChainType);
    lWallet.setWif(iWif);
    lWallet.setP2WPKHAddress(lP2WPKH);
    lWallet.setP2SH_P2WPKHAddress(lP2SH_P2WPKH);
    lWallet.setP2PKHAddress(lP2PKH);
    lWallet.setPrivateKey(iPrivateKey);
    lWallet.setPublicKey(lPublicKey);
    lWallet.setShortNameType(QStringLiteral("BTC"));
    lWallet.setLongNameType(QStringLiteral("Bitcoin"));
    lWallet.setValue(QStringLiteral("0"));

    return lWallet;
}
