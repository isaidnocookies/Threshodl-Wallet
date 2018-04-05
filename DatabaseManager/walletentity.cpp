#include "walletentity.h"

#include <QVariant>

WalletEntity::WalletEntity() : Entity(PersistenceType::PERSISTENCE_TYPE_WALLET) {
    this->currency = currency;
    this->amount = "0";
}

WalletEntity::WalletEntity(RepositoryType repositoryType) : Entity(PersistenceType::PERSISTENCE_TYPE_WALLET, repositoryType) {
    this->currency = currency;
    this->amount = "0";
}

WalletEntity::WalletEntity(WalletType type, QString owner, QString currency) : Entity(PersistenceType::PERSISTENCE_TYPE_WALLET) {
    this->type = type;
    this->owner = owner;
    this->currency = currency;
    this->amount = "0";
}

WalletEntity::WalletEntity(WalletType type, QString owner, QString currency, RepositoryType repositoryType) : Entity(PersistenceType::PERSISTENCE_TYPE_WALLET, repositoryType) {
    this->type = type;
    this->owner = owner;
    this->currency = currency;
    this->amount = "0";
}

WalletEntity::WalletEntity(QSqlRecord sqlrecord) : Entity(PersistenceType::PERSISTENCE_TYPE_WALLET, RepositoryType::REPOSITORY_TYPE_SQLITE)  {
    if(sqlrecord.contains("walletId")) {
        this->id = sqlrecord.value(sqlrecord.indexOf("walletId")).toInt();
    }
    if(sqlrecord.contains("walletType")) {
        int type = sqlrecord.value(sqlrecord.indexOf("walletType")).toInt();
        this->type = (WalletType)type;
    }
    if(sqlrecord.contains("walletOwner")) {
        this->owner = sqlrecord.value(sqlrecord.indexOf("walletOwner")).toString();
    }
    if(sqlrecord.contains("walletCurrency")) {
        this->currency = sqlrecord.value(sqlrecord.indexOf("walletCurrency")).toString();
    }
    if(sqlrecord.contains("walletAmount")) {
        this->amount = sqlrecord.value(sqlrecord.indexOf("walletAmount")).toString();
    }
}

/**
 * Get the unique identificator for the entity
 *
 * @brief WalletEntity::getId
 * @return
 */
int WalletEntity::getId() {
    return this->id;
}

WalletType WalletEntity::getType() {
    return this->type;
}

QString WalletEntity::getOwner() {
    return this->owner;
}

QString WalletEntity::getCurrency() {
    return this->currency;
}

QString WalletEntity::getAmount() {
    return this->amount;
}

QList<BillEntity*> WalletEntity::getBills() {
    return this->bills;
}

void WalletEntity::setType(WalletType type) {
    this->type = type;
}

void WalletEntity::setOwner(QString owner) {
    this->owner = owner;
}

void WalletEntity::setCurrency(QString currency) {
    this->currency = currency;
}

void WalletEntity::setAmount(QString amount) {
    this->amount = amount;
}

void WalletEntity::addBill(BillEntity *bill) {
    if (!this->bills.contains(bill)) {
        bill->setOwner(this->owner);
        bill->setWallet(this);
        this->bills.append(bill);
    }
}

void WalletEntity::removeBill(BillEntity *bill) {
    if (this->bills.contains(bill)) {
        this->bills.removeOne(bill);
    }
}

void WalletEntity::addBillWad(QList<BillEntity *> wad) {
    foreach (BillEntity* bill, wad) {
        if (!this->bills.contains(bill)) {
            this->bills.append(bill);
        }
    }
}

/**
 * Update the id value after a persist operation
 *
 * @brief WalletEntity::updateAfterPersist
 * @param id
 */
void WalletEntity::updateAfterPersist(int id) {
    this->id = id;
}

/**
 * Fetching in `eager` mode means the instantiation of the objects
 * also creates linked objects when reading from the database
 *
 * @brief WalletEntity::getSQLSelect
 * @param fetchMode
 * @return
 */
QString WalletEntity::getSQLSelect(FetchMode fetchMode) {
    QString query;
    switch(fetchMode) {
        case FetchMode::FETCH_MODE_EAGER:
            query = "select \
                    wallets.id as walletId, \
                    wallets.type as walletType, \
                    wallets.owner as walletOwner, \
                    wallets.currency as walletCurrency, \
                    wallets.amount as walletAmount, \
                    bills.id as id, \
                    bills.owner as owner, \
                    bills.address as address, \
                    bills.publicKey as publicKey, \
                    bills.privateKey as privateKeys, \
                    bills.currency as currency, \
                    bills.amount as amount \
                    from wallets \
                    left join \
                    bills on wallets.id = bills.wallet";
        break;
        case FetchMode::FETCH_MODE_LAZY:
            query = "select \
                    wallets.id as walletId, \
                    wallets.type as walletType, \
                    wallets.owner as walletOwner, \
                    wallets.currency as walletCurrency, \
                    wallets.amount as walletAmount \
                    from wallets";
    }
    return query;
}

QString WalletEntity::getSQLInsert() {
    QString query = "INSERT INTO wallets (type,owner,currency,amount) VALUES ";
    query += "(" + QString::number(this->getType(), 10) + ",'" + this->getOwner() + "','" + this->getCurrency() + "','" + this->getAmount() + "');";
    return query;
}

QString WalletEntity::getSQLUpdate(QString amount) {
    QString query;
    query = "UPDATE wallets set amount = " + amount + "  where id = " + QString::number(this->getId());
    return query;
}
