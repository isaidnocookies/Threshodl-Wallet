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
