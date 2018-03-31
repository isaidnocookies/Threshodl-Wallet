#include "bill.h"

Bill::Bill(Wallet wallet)
{
    this->mWallet = wallet;
    // Generate The wallet data accordingly to the requested coin
    // ...
    this->mAddress = "addres-----rfghsakatw4o8atoref";
    this->mPublicKey = "public-----mgbjsdthsasdlfkseof";
    this->mPrivateKey = "private-----k5g2k3j45g2k3atoref";
}

Wallet Bill::getWallet() {
    return this->mWallet;
}

QString Bill::getAddress() {
    return this->mAddress;
}

QString Bill::getPublicKey() {
    return this->mPublicKey;
}

QString Bill::getAmount() {
    return this->mAmount;
}
