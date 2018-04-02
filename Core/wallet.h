#ifndef WALLET_H
#define WALLET_H

#include <QString>
#include <QObject>
#include <QList>
#include "bill.h"
#include "utils.h"

// Forward declaration to deal with class interdependency
class Bill;

class Wallet : public QObject
{
    Q_OBJECT
    Q_PROPERTY(WalletMode type READ getType)
    Q_PROPERTY(QString currency READ getCurrency)
    Q_PROPERTY(QString owner READ getOwner)
    Q_PROPERTY(QString amount READ getAmount NOTIFY amountChanged)
    Q_PROPERTY(QList<QObject*> bills READ listBills NOTIFY billsChanged)

private:
    WalletMode mType;
    QString mCurrency;
    QString mOwner;
    QString mAmount;
    QList<Bill*> mBills;
    // Internal usage methods
    // ...

public:
    Wallet(WalletMode type, QString currency, QString owner);
    WalletMode getType();
    QString getCurrency();
    QString getOwner();
    QString getAmount();
    QList<Bill*> getBills();
    QList<QObject*> listBills();
    void addBill(Bill* bill);
    void addBillWad(QList<Bill*> bills);

signals:
    void amountChanged();
    void countChanged();
    void billsChanged();
};

#endif // WALLET_H
