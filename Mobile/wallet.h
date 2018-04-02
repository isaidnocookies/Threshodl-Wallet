#ifndef WALLET_H
#define WALLET_H

#include <QString>
#include <QObject>
#include <QList>
#include "bill.h"

// Forward declaration to deal with class interdependency
class Bill;

class Wallet : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int type READ getType)
    Q_PROPERTY(QString currency READ getCurrency)
    Q_PROPERTY(QString owner READ getOwner)
    Q_PROPERTY(QString amount READ getAmount NOTIFY amountChanged)
    Q_PROPERTY(int count READ getCount NOTIFY countChanged)
    Q_PROPERTY(QList<QObject*> bills READ listBills NOTIFY billsChanged)

private:
    int mType;
    QString mCurrency;
    QString mOwner;
    QString mAmount;
    QList<Bill*> mBills;
    // Internal usage methods
    // ...

public:
    Wallet(int type, QString currency, QString owner);
    int getType();
    QString getCurrency();
    QString getOwner();
    QString getAmount();
    int getCount();
    QList<Bill*> getBills();
    QList<QObject*> listBills();
    void addBill(Bill* bill);

signals:
    void amountChanged();
    void countChanged();
    void billsChanged();
};

#endif // WALLET_H
