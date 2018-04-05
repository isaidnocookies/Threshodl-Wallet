#include "walletentity.h"

#include <QVariant>

WalletEntity::WalletEntity() : Entity(PersistenceType::PERSISTENCE_TYPE_WALLET) {
    this->mAmount = "0";
}

WalletEntity::WalletEntity(WalletType type, QString owner, QString currency) : Entity(PersistenceType::PERSISTENCE_TYPE_WALLET) {
    this->mType = type;
    this->mOwner = owner;
    this->mCurrency = currency;
    this->mAmount = "0";
}

WalletEntity::WalletEntity(QSqlRecord sqlrecord) : Entity(PersistenceType::PERSISTENCE_TYPE_WALLET)  {
    if(sqlrecord.contains("walletId")) {
        this->mId = sqlrecord.value(sqlrecord.indexOf("walletId")).toInt();
    }
    if(sqlrecord.contains("walletType")) {
        int type = sqlrecord.value(sqlrecord.indexOf("walletType")).toInt();
        this->mType = (WalletType)type;
    }
    if(sqlrecord.contains("walletOwner")) {
        this->mOwner = sqlrecord.value(sqlrecord.indexOf("walletOwner")).toString();
    }
    if(sqlrecord.contains("walletCurrency")) {
        this->mCurrency = sqlrecord.value(sqlrecord.indexOf("walletCurrency")).toString();
    }
    if(sqlrecord.contains("walletAmount")) {
        this->mAmount = sqlrecord.value(sqlrecord.indexOf("walletAmount")).toString();
    }
}

WalletEntity::~WalletEntity() {

}

/**
 * Get the unique identificator for the entity
 *
 * @brief WalletEntity::getId
 * @return
 */
int WalletEntity::getId() {
    return this->mId;
}

WalletType WalletEntity::getType() {
    return this->mType;
}

QString WalletEntity::getOwner() {
    return this->mOwner;
}

QString WalletEntity::getCurrency() {
    return this->mCurrency;
}

QString WalletEntity::getAmount() {
    return this->mAmount;
}

QList<BillEntity*> WalletEntity::getBills() {
    return this->mBills;
}

/**
 * Get the list of Bills contained in the user wallet in a format
 * able to be understood at QML side
 *
 * @brief Wallet::listBills
 * @return
 */
QList<QObject*> WalletEntity::listBills() {
    QList<QObject*> list;
    foreach(BillEntity* bill, this->mBills) {
        list.append(bill);
    }
    return list;
}

void WalletEntity::setType(WalletType type) {
    this->mType = type;
}

void WalletEntity::setOwner(QString owner) {
    this->mOwner = owner;
}

void WalletEntity::setCurrency(QString currency) {
    this->mCurrency = currency;
}

void WalletEntity::setAmount(QString amount) {
    this->mAmount = amount;
}

void WalletEntity::addBill(BillEntity *bill) {
    if (!this->mBills.contains(bill)) {
        bill->setOwner(this->mOwner);
        bill->setWallet(this);
        this->mBills.append(bill);
    }
}

void WalletEntity::removeBill(BillEntity *bill) {
    if (this->mBills.contains(bill)) {
        this->mBills.removeOne(bill);
    }
}

void WalletEntity::addBillWad(QList<BillEntity *> wad) {
    foreach (BillEntity* bill, wad) {
        if (!this->mBills.contains(bill)) {
            this->mBills.append(bill);
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
    this->mId = id;
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
    query += "(" + QString::number(this->mType, 10) + ",'" + this->mOwner + "','" + this->mCurrency + "','" + this->mAmount + "');";
    return query;
}

QString WalletEntity::getSQLUpdate(QString amount) {
    QString query;
    query = "UPDATE wallets set amount = " + amount + "  where id = " + QString::number(this->mId);
    return query;
}
