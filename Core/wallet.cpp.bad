#include "wallet.h"

/**
 * Create a new user wallet.
 * User wallets are the higher abstraction object to deal with amounts of any currency
 * Each user wallet holds a list of bills(wallets or microwallets), the total amount,
 * the currency, the owner nickname and the type form modes A(0) and B(1)
 *
 * @brief Wallet::Wallet
 * @param type  Wallet mode A or B (0 or 1)
 * @param currency Currency type
 * @param owner Owner nickname
 */
Wallet::Wallet(WalletType type, QString currency, QString owner)
{
    this->mType = type;
    this->mCurrency = currency;
    this->mOwner = owner;
    this->mAmount = "0";
}

/**
 * Get the wallet type or mode
 *
 * @brief Wallet::getType
 * @return WalletType
 */
WalletType Wallet::getType() {
    return this->mType;
}

/**
 * Get wallet currency
 *
 * @brief Wallet::getCurrency
 * @return QString
 */
QString Wallet::getCurrency() {
    return this->mCurrency;
}

/**
 * Get wallet owner
 *
 * @brief Wallet::getOwner
 * @return QString
 */
QString Wallet::getOwner() {
    return this->mOwner;
}

/**
 * Get wallet amount. This amont is being held as an independent
 * value to speed readings, but its actual value needs to be event based
 * in order to be recalculated from the Bills(real wallets or microwallets)
 *
 * @brief Wallet::getAmount
 * @return QString
 */
QString Wallet::getAmount() {
    return this->mAmount;
}

/**
 * Get the list of Bills contained in the user wallet
 *
 * @brief Wallet::getBills
 * @return QList<Bill*>
 */
QList<Bill*> Wallet::getBills() {
    return this->mBills;
}

/**
 * Get the list of Bills contained in the user wallet in a format
 * able to be understood at QML side
 *
 * @brief Wallet::listBills
 * @return
 */
QList<QObject*> Wallet::listBills() {
    QList<QObject*> list;
    foreach(Bill* bill, this->mBills) {
        list.append(bill);
    }
    return list;
}

/**
 * Adding a single bill(real wallet or microwallet) to the wallet
 * TODO: add security constraints
 *
 * @brief Wallet::addBill
 * @param bill The Bill to be included
 */
void Wallet::addBill(Bill* bill) {
    if (!this->mBills.contains(bill)) {
        this->mBills.append(bill);
    }
}

/**
 * Adding a single bill(real wallet or microwallet) to the wallet
 * TODO: add security constraints
 *
 * @brief Wallet::addBillWad
 * @param bills The Bills to be added into the user wallet
 */
void Wallet::addBillWad(QList<Bill*> bills) {
    foreach(Bill* bill, bills) {
        if (!this->mBills.contains(bill)) {
            this->mBills.append(bill);
        }
    }
}
