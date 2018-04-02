#include "wallet.h"

Wallet::Wallet(int type, QString currency, QString owner)
{
    this->mType = type;
    this->mCurrency = currency;
    this->mOwner = owner;
    this->mAmount = "0";
}

int Wallet::getType() {
    return this->mType;
}

QString Wallet::getCurrency() {
    return this->mCurrency;
}

QString Wallet::getOwner() {
    return this->mOwner;
}

QString Wallet::getAmount() {
    return this->mAmount;
}

int Wallet::getCount() {
    return this->mBills.length();
}

QList<Bill*> Wallet::getBills() {
    return this->mBills;
}

QList<QObject*> Wallet::listBills() {
    QList<QObject*> list;
    foreach(Bill* bill, this->mBills) {
        list.append(bill);
    }
    return list;
}

void Wallet::addBill(Bill* bill) {
    if (!this->mBills.contains(bill)) {
        this->mBills.append(bill);
    }
}
