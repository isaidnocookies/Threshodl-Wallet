#include "billentity.h"

#include <QDebug>
#include "bitcoininterface.h"

BillEntity::BillEntity(WalletEntity* wallet) : Entity(PersistenceType::PERSISTENCE_TYPE_BILL) {
    this->mAmount = "0";
    this->mWallet = wallet;
    if (wallet != NULL) {
        QString currencyType = wallet->getCurrency();
        this->mOwner = wallet->getOwner();
        this->mCurrency = currencyType;
        // Just BTC at this time
        switch(this->mWallet->getType()) {
            case WalletType::WALLET_TYPE_DARK:
                // Generate the Bill at server side
                // ...
            break;
            default:
                if (QString::compare("BTC", this->getCurrency(), Qt::CaseInsensitive) == 0) {
                    BitcoinInterface btcInterface;
                    QList<BitcoinWalletRef> wallets = btcInterface.createWallets(1, true);
                    BitcoinWalletRef localWallet = wallets.at(0);
                    // Using the classic address this time
                    this->mAddress = localWallet->Addresses["P2PKH"];
                    this->mPublicKey = localWallet->PublicKey;
                    this->mPrivateKey = localWallet->PrivateKey;
                }
        }
    }
}

BillEntity::BillEntity(QSqlRecord sqlrecord, WalletEntity* wallet) : Entity(PersistenceType::PERSISTENCE_TYPE_BILL) {
    if(sqlrecord.contains("billId")) {
        this->mId = sqlrecord.value(sqlrecord.indexOf("billId")).toInt();
    }
    if(sqlrecord.contains("billOwner")) {
        this->mOwner = sqlrecord.value(sqlrecord.indexOf("billOwner")).toString();
    }
    if(sqlrecord.contains("billAddress")) {
        this->mAddress = sqlrecord.value(sqlrecord.indexOf("billAddress")).toString();
    }
    if(sqlrecord.contains("billPublicKey")) {
        this->mPublicKey = sqlrecord.value(sqlrecord.indexOf("billPublicKey")).toString();
    }
    if(sqlrecord.contains("billPrivateKey")) {
        this->mPrivateKey = sqlrecord.value(sqlrecord.indexOf("billPrivateKey")).toString();
    }
    if(sqlrecord.contains("billCurrency")) {
        this->mCurrency = sqlrecord.value(sqlrecord.indexOf("billCurrency")).toString();
    }
    if(sqlrecord.contains("billAmount")) {
        this->mAmount = sqlrecord.value(sqlrecord.indexOf("billAmount")).toString();
    }
    this->mWallet = wallet;
}

BillEntity::~BillEntity() {

}

/**
 * Get the unique identificator for the entity
 *
 * @brief BillEntity::getId
 * @return
 */
int BillEntity::getId() {
    return this->mId;
}

WalletEntity* BillEntity::getWallet() {
    return this->mWallet;
}

QString BillEntity::getOwner() {
    return this->mOwner;
}

QString BillEntity::getAddress() {
    return this->mAddress;
}

QString BillEntity::getPublicKey() {
    return this->mPublicKey;
}

QString BillEntity::getPrivateKey() {
    return this->mPrivateKey;
}

QString BillEntity::getCurrency() {
    return this->mCurrency;
}

QString BillEntity::getAmount() {
    return this->mAmount;
}

void BillEntity::setWallet(WalletEntity* wallet) {
    this->mWallet = wallet;
}

void BillEntity::setOwner(QString owner) {
    this->mOwner = owner;
}

void BillEntity::setAddress(QString address) {
    this->mAddress = address;
}

void BillEntity::setPublicKey(QString publicKey) {
    this->mPublicKey = publicKey;
}

void BillEntity::setPrivateKey(QString privateKey) {
    this->mPrivateKey = privateKey;
}

void BillEntity::setCurrency(QString currency) {
    this->mCurrency = currency;
}

void BillEntity::setAmount(QString amount) {
    this->mAmount = amount;
}

/**
 * Update the id value after a persist operation
 *
 * @brief BillEntity::updateAfterPersist
 * @param id
 */
void BillEntity::updateAfterPersist(int id) {
    this->mId = id;
}

/**
 * Fetching in `eager` mode means the instantiation of the objects
 * also creates linked objects when reading from the database
 *
 * @brief BillEntity::getSQLSelect
 * @param fetchMode
 * @return
 */
QString BillEntity::getSQLSelect(FetchMode fetchMode) {
    QString query;
    switch(fetchMode) {
        case FetchMode::FETCH_MODE_EAGER:
            query = "select \
                    bills.id as billId, \
                    bills.owner as billOwner, \
                    bills.address as billAddress, \
                    bills.publicKey as billPublicKey, \
                    bills.privateKey as billPrivateKey, \
                    bills.currency as billCurrency, \
                    bills.amount as billAmount, \
                    wallets.id as walletId, \
                    wallets.type as walletType, \
                    wallets.owner as walletOwner, \
                    wallets.currency as walletCurrency, \
                    wallets.amount as walletAmount \
                    from bills \
                    left join \
                    wallets on wallets.id = bills.wallet";
        break;
        case FetchMode::FETCH_MODE_LAZY:
            query = "select \
                    bills.id as billId, \
                    bills.owner as billOwner, \
                    bills.wallet as billWallet, \
                    bills.address as billAddress, \
                    bills.publicKey as billPublicKey, \
                    bills.privateKey as billPrivateKey, \
                    bills.currency as billCurrency, \
                    bills.amount as billAmount, \
                    from bills";
    }
    return query;
}

QString BillEntity::getSQLInsert() {
    QString query = "INSERT INTO bills (owner,wallet,address,publicKey,privateKey,currency,amount) VALUES ";
    query += "('" + this->mOwner + "'," + QString::number(this->mWallet->getId()) + ",'" + this->mAddress + "','" + this->mPublicKey + "','" + this->mPrivateKey + "','" + this->mCurrency + "','" + this->mAmount + "');";
    return query;
}

QString BillEntity::getSQLUpdate(QString amount) {
    QString query;
    query = "UPDATE bills set amount = " + amount + "  where id = " + QString::number(this->mId);
    return query;
}
