#include "bitcoinwallet.h"

#include <QJsonDocument>

extern "C" {
#include <btc/chainparams.h>
#include <btc/ecc.h>
#include <btc/tool.h>
}

#define     kBTCStringBufferSize    128

BitcoinWallet::BitcoinWallet()
    : Wallet()
{
}

BitcoinWallet::BitcoinWallet(const QByteArray iData)
{
    QVariantMap lDataMap;
    lDataMap = QJsonDocument::fromBinaryData(iData).toVariant().toMap();
    _baseClassFromData(lDataMap);
    _bitcoinWalletClassFromData(lDataMap);
}

BitcoinWallet::~BitcoinWallet()
{ }

BitcoinWallet::ChainType BitcoinWallet::chain() const
{ return mChain; }

QByteArray BitcoinWallet::wif() const
{ return mWif; }

QByteArray BitcoinWallet::p2pkhAddress() const
{ return address(); }

QByteArray BitcoinWallet::p2wpkhAddress()
{
    if( mP2WPKHAddress.isEmpty() ) _calculateAddresses();
    if( ! mP2WPKHAddress.isEmpty() ) return mP2WPKHAddress;
    return QByteArray();
}

QByteArray BitcoinWallet::p2sh_p2wpkhAddress()
{
    if( mP2SH_P2WPKHAddress.isEmpty() ) _calculateAddresses();
    if( ! mP2SH_P2WPKHAddress.isEmpty() ) return mP2SH_P2WPKHAddress;
    return QByteArray();
}

QByteArray BitcoinWallet::toData() const
{ return _baseClassToData(_bitcoinWalletClassToData(QVariantMap())); }

QSharedPointer<BitcoinWallet> BitcoinWallet::createNewBitcoinWallet(ChainType iChainType)
{
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
        return QSharedPointer<BitcoinWallet>();

    lWif.resize(qstrlen(lWif.constData())+1);
    lPrivateKey.resize(qstrlen(lPrivateKey.constData())+1);
    return fromWifAndPrivateKey(lWif,lPrivateKey,iChainType);
}

QSharedPointer<BitcoinWallet> BitcoinWallet::fromWifAndPrivateKey(const QByteArray iWif, const QByteArray iPrivateKey, ChainType iChainType)
{
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
        return QSharedPointer<BitcoinWallet>();

    lPublicKey.resize(lPublicSize);

    // Now Generate addresses
    QByteArray                  lP2PKH{kBTCStringBufferSize,0};
    QByteArray                  lP2WPKH{kBTCStringBufferSize,0};
    QByteArray                  lP2SH_P2WPKH{kBTCStringBufferSize,0};

    if( ! addresses_from_pubkey(lChain, lPublicKey.constData(), lP2PKH.data(), lP2SH_P2WPKH.data(), lP2WPKH.data()) )
        return QSharedPointer<BitcoinWallet>();

    lP2PKH.resize(qstrlen(lP2PKH.constData()) + 1);
    lP2WPKH.resize(qstrlen(lP2WPKH.constData()) + 1);
    lP2SH_P2WPKH.resize(qstrlen(lP2SH_P2WPKH.constData()) + 1);

    QSharedPointer<BitcoinWallet>   lWallet = QSharedPointer<BitcoinWallet>(new BitcoinWallet);
    lWallet->mChain                 = iChainType;
    lWallet->mWif                   = iWif;
    lWallet->mP2WPKHAddress         = lP2WPKH;
    lWallet->mP2SH_P2WPKHAddress    = lP2SH_P2WPKH;
    lWallet->mAddress               = lP2PKH;
    lWallet->mPrivateKey            = iPrivateKey;
    lWallet->mPublicKey             = lPublicKey;
    lWallet->mShortNameType         = QStringLiteral("BTC");
    lWallet->mLongNameType          = QStringLiteral("Bitcoin");
    lWallet->mValue                 = QStringLiteral("0");
    return lWallet;
}

QVariantMap BitcoinWallet::_bitcoinWalletClassToData(QVariantMap iMap) const
{
    iMap["chain"]       = static_cast<unsigned int>(mChain);
    iMap["wif"]         = mWif.toBase64();
    return iMap;
}

void BitcoinWallet::_bitcoinWalletClassFromData(QVariantMap iDataMap)
{
    mChain      = static_cast<ChainType>(iDataMap["chain"].toUInt());
    mWif        = QByteArray::fromBase64(iDataMap["wif"].toByteArray());
}

void BitcoinWallet::_calculateAddresses()
{
    if( mPublicKey.isEmpty() ) return;

    const btc_chainparams *     lChain              = &btc_chainparams_main;

    switch( mChain ) {
    case ChainType::None:
        // Error
        return;

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

    // Now Generate addresses
    QByteArray                  lP2PKH{kBTCStringBufferSize,0};
    QByteArray                  lP2WPKH{kBTCStringBufferSize,0};
    QByteArray                  lP2SH_P2WPKH{kBTCStringBufferSize,0};

    if( ! addresses_from_pubkey(lChain, mPublicKey.constData(), lP2PKH.data(), lP2SH_P2WPKH.data(), lP2WPKH.data()) )
        return;

    // For now we ignore P2PKH
    lP2WPKH.resize(qstrlen(lP2WPKH.constData()) + 1);
    lP2SH_P2WPKH.resize(qstrlen(lP2SH_P2WPKH.constData()) + 1);

    mP2WPKHAddress = lP2WPKH;
    mP2SH_P2WPKHAddress = lP2SH_P2WPKH;
    return;
}
