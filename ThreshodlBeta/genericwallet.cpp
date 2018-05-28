#include "genericwallet.h"
#include "bitcoinwallet.h"

#include <QMutex>

extern "C" {
#include <btc/chainparams.h>
#include <btc/ecc.h>
#include <btc/tool.h>
#include <btc/utils.h>
}

GenericWallet::GenericWallet() : Wallet()
{ }

GenericWallet::GenericWallet(const QByteArray iData) : Wallet(iData)
{ }

GenericWallet::GenericWallet(const Wallet &iOther) : Wallet(iOther)
{ }

GenericWallet::GenericWallet(const WalletDataCore &iOther) : Wallet(iOther)
{ }

GenericWallet GenericWallet::createWallet(QString iShortname, GenericWallet::ChainType iChainType)
{
    if (iShortname == "BTC") {
        return BitcoinWallet::createNewBitcoinWallet(static_cast<BitcoinWallet::ChainType>(iChainType));
    } else if (iShortname == "ETH") {
        return createEthWallet(iChainType);
    } else if (iShortname == "LTC") {
        return createLitecoinWallet(iChainType);
    } else if (iShortname == "DASH") {
        return createDashWallet(iChainType);
    } else if (iShortname == "ZEC") {
        return createZCashWallet(iChainType);
    }

    return GenericWallet{};
}

QByteArray GenericWallet::generateBtcWifFromPrivateKey(const QByteArray iPrivateKey, GenericWallet::ChainType iChainType)
{
    return BitcoinWallet::generateWifFromPrivateKey(iPrivateKey, static_cast<BitcoinWallet::ChainType>(iChainType));
}

GenericWallet GenericWallet::fromBtcWifAndPrivateKey(const QByteArray iWif, const QByteArray iPrivateKey, GenericWallet::ChainType iChainType)
{
    return BitcoinWallet::fromWifAndPrivateKey(iWif, iPrivateKey, static_cast<BitcoinWallet::ChainType>(iChainType));
}

GenericWallet GenericWallet::createDashWallet(GenericWallet::ChainType iChainType)
{
    return BitcoinWallet::createNewBitcoinWallet(static_cast<BitcoinWallet::ChainType>(iChainType));
}

GenericWallet GenericWallet::createLitecoinWallet(GenericWallet::ChainType iChainType)
{
    return BitcoinWallet::createNewBitcoinWallet(static_cast<BitcoinWallet::ChainType>(iChainType));
}

GenericWallet GenericWallet::createZCashWallet(GenericWallet::ChainType iChainType)
{
    return BitcoinWallet::createNewBitcoinWallet(static_cast<BitcoinWallet::ChainType>(iChainType));
}

GenericWallet GenericWallet::createEthWallet(GenericWallet::ChainType iChainType)
{
    return BitcoinWallet::createNewBitcoinWallet(static_cast<BitcoinWallet::ChainType>(iChainType));
}
