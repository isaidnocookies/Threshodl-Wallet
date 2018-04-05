#include "billentity.h"

#include <QDebug>

BillEntity::BillEntity(WalletEntity* wallet) : Entity(PersistenceType::PERSISTENCE_TYPE_BILL){
    this->wallet = wallet;
    this->currency = wallet->getCurrency();
    this->amount = "0";
}

BillEntity::BillEntity(WalletEntity* wallet, RepositoryType repositoryType) : Entity(PersistenceType::PERSISTENCE_TYPE_BILL, repositoryType){
    this->wallet = wallet;
    this->currency = wallet->getCurrency();
    this->amount = "0";
}

BillEntity::BillEntity(QSqlRecord sqlrecord, WalletEntity* wallet) : Entity(PersistenceType::PERSISTENCE_TYPE_BILL, RepositoryType::REPOSITORY_TYPE_SQLITE) {
    if(sqlrecord.contains("billId")) {
        this->id = sqlrecord.value(sqlrecord.indexOf("billId")).toInt();
    }
    if(sqlrecord.contains("billOwner")) {
        this->owner = sqlrecord.value(sqlrecord.indexOf("billOwner")).toString();
    }
    if(sqlrecord.contains("billAddress")) {
        this->address = sqlrecord.value(sqlrecord.indexOf("billAddress")).toString();
    }
    if(sqlrecord.contains("billPublicKey")) {
        this->publicKey = sqlrecord.value(sqlrecord.indexOf("billPublicKey")).toString();
    }
    if(sqlrecord.contains("billPrivateKey")) {
        this->privateKey = sqlrecord.value(sqlrecord.indexOf("billPrivateKey")).toString();
    }
    if(sqlrecord.contains("billCurrency")) {
        this->currency = sqlrecord.value(sqlrecord.indexOf("billCurrency")).toString();
    }
    if(sqlrecord.contains("billAmount")) {
        this->amount = sqlrecord.value(sqlrecord.indexOf("billAmount")).toString();
    }
    this->wallet = wallet;
}

/**
 * Get the unique identificator for the entity
 *
 * @brief BillEntity::getId
 * @return
 */
int BillEntity::getId() {
    return this->id;
}

WalletEntity* BillEntity::getWallet() {
    return this->wallet;
}

QString BillEntity::getOwner() {
    return this->owner;
}

QString BillEntity::getAddress() {
    return this->address;
}

QString BillEntity::getPublicKey() {
    return this->publicKey;
}

QString BillEntity::getPrivateKey() {
    return this->privateKey;
}

QString BillEntity::getCurrency() {
    return this->currency;
}

QString BillEntity::getAmount() {
    return this->amount;
}

void BillEntity::setWallet(WalletEntity* wallet) {
    this->wallet = wallet;
}

void BillEntity::setOwner(QString owner) {
    this->owner = owner;
}

void BillEntity::setAddress(QString address) {
    this->address = address;
}

void BillEntity::setPublicKey(QString publicKey) {
    this->publicKey = publicKey;
}

void BillEntity::setPrivateKey(QString privateKey) {
    this->privateKey = privateKey;
}

void BillEntity::setCurrency(QString currency) {
    this->currency = currency;
}

void BillEntity::setAmount(QString amount) {
    this->amount = amount;
}

/**
 * Update the id value after a persist operation
 *
 * @brief BillEntity::updateAfterPersist
 * @param id
 */
void BillEntity::updateAfterPersist(int id) {
    this->id = id;
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
    query += "('" + this->getOwner() + "'," + QString::number(this->wallet->getId()) + ",'" + this->getAddress() + "','" + this->getPublicKey() + "','" + this->getPrivateKey() + "','" + this->getCurrency() + "','" + this->getAmount() + "');";
    return query;
}

QString BillEntity::getSQLUpdate(QString amount) {
    QString query;
    query = "UPDATE bills set amount = " + amount + "  where id = " + QString::number(this->getId());
    return query;
}
