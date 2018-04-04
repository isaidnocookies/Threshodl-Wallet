#include "walletentity.h"

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
