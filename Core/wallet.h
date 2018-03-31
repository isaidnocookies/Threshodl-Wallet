#ifndef WALLET_H
#define WALLET_H

#include <QString>
#include <QObject>
#include <QSet>
#include "bill.h"

class Wallet : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int type READ getType)
    Q_PROPERTY(QString currency READ getCurrency)
    Q_PROPERTY(QString owner READ getOwner)
    Q_PROPERTY(QString amount READ getAmount NOTIFY amountChanged)
    Q_PROPERTY(QSet<Bill> bills READ getBills NOTIFY billsChanged)

private:
    int mType;
    QString mCurrency;
    QString mOwner;
    QString mAmount;
    QSet<Bill> mBills;
    // Internal usage methods
    // ...

public:
    Wallet(int type, QString currency, QString owner);
    int getType();
    QString getCurrency();
    QString getOwner();
    QString getAmount();
    QSet<Bill> getBills();
    void addBill(Bill bill);

signals:
    void amountChanged();
    void billsChanged();
};

#endif // WALLET_H
