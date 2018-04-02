#include "bill.h"
#include "bitcoininterface.h"

#include <QDebug>

/**
 * Generate the bill(real wallet) data locally accordingly to the requested coin
 * Bills representing real wallets are completely owned by the customer
 *
 * @brief Bill::Bill Base constructor for local wallet generation
 * @param wallet Pointer to the user wallet holding the bill(real wallet)
 */
Bill::Bill(Wallet* wallet)
{
    this->mWallet = wallet;
    this->mAmount = "0";
    this->mCurrency = wallet->getCurrency();
    // Just BTC at this time
    if (QString::compare(this->getCurrency(), "BTC", Qt::CaseInsensitive) == 0) {
        qDebug() << "sdfg sdfgsdfg";
        BitcoinInterface btcInterface;
        QList<BitcoinWalletRef> wallets = btcInterface.createWallets(1, true);
        BitcoinWalletRef localWallet = wallets.at(0);
        this->mAddress = localWallet->Addresses["P2PKH"];
        this->mPublicKey = localWallet->PublicKey;
        this->mPrivateKey = localWallet->PrivateKey;
    }
}

/**
 * Instantiate the bill(microwallet) using remote generated data accordingly to the requested coin
 * Bills representing microwallets have uncompleted private keys.
 *
 * @brief Bill::Bill Base constructor for wallet generation with remote data
 * @param wallet Pointer to the user wallet holding the bill(microwallet)
 * @param address The microwallet address
 * @param publickey The microwallet public key
 * @param privateKey The microwallet private key shard
 * @param currency The currency type for the bill(microwallet)
 * @param amount The amount of currency available in the bill(microwallet)
 */
Bill::Bill(Wallet* wallet, QString address, QString publicKey, QString privateKey, QString currency, QString amount)
{
    this->mWallet = wallet;
    // Generate The wallet data accordingly to the requested coin
    // ...
    this->mAddress = address;
    this->mPublicKey = publicKey;
    this->mPrivateKey = privateKey;
    this->mCurrency = currency;
    this->mAmount = amount;
}

/**
 * Get the user wallet the bill belongs to.
 *
 * @brief Bill::getWallet
 * @return Wallet
 */
Wallet* Bill::getWallet() {
    return this->mWallet;
}

/**
 * Get the bill(real wallet or microwallet) address
 *
 * @brief Bill::getAddress
 * @return QString
 */
QString Bill::getAddress() {
    return this->mAddress;
}

/**
 * Get the bill(real wallet or microwallet) public key
 *
 * @brief Bill::getPublicKey
 * @return QString
 */
QString Bill::getPublicKey() {
    return this->mPublicKey;
}

/**
 * Get the bill(real wallet or microwallet) currency
 *
 * @brief Bill::getCurrency
 * @return QString
 */
QString Bill::getCurrency() {
    return this->mCurrency;
}

/**
 * Get the bill(real wallet or microwallet) amount
 *
 * @brief Bill::getAmount
 * @return QString
 */
QString Bill::getAmount() {
    return this->mAmount;
}
