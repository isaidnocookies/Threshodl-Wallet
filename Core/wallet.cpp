#include "wallet.h"

Wallet::Wallet(int type, QString currency, QString owner)
{
    this->mType = type;
    this->mCurrency = currency;
    this->mOwner = owner;
    this->mAmount = "0";
}

QString Wallet::getType() {
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

QSet<Bill> Wallet::getBills() {
    return this->mBills;
}

void Wallet::addBill(Bill bill) {
    this->mBills.insert(bill);
}
