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
    GenericWallet::ChainType lNetwork = iChainType;

    if (iShortname.at(0) == "t") {
        lNetwork = GenericWallet::ChainType::TestNet;
    } else {
        lNetwork = GenericWallet::ChainType::Main;
    }

    if (iShortname == "BTC") {
        return BitcoinWallet::createNewBitcoinWallet(static_cast<BitcoinWallet::ChainType>(lNetwork));
    } else if (iShortname == "tBTC") {
        return BitcoinWallet::createNewBitcoinWallet(static_cast<BitcoinWallet::ChainType>(lNetwork));
    }else if (iShortname == "ETH") {
        return createEthWallet(lNetwork);
    } else if (iShortname == "LTC") {
        return createLitecoinWallet(lNetwork);
    } else if (iShortname == "DASH") {
        return createDashWallet(lNetwork);
    } else if (iShortname == "ZEC") {
        return createZCashWallet(lNetwork);
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
