#ifndef WALLETENTITY_H
#define WALLETENTITY_H

#include "walletentity.h"
#include "entity.h"
#include <QObject>
#include <QString>
#include <QSqlRecord>
#include <QList>
#include "billentity.h"

class BillEntity;

class WalletEntity : public Entity, public QObject
{
    Q_OBJECT
    Q_PROPERTY(WalletMode type READ getType)
    Q_PROPERTY(QString currency READ getCurrency)
    Q_PROPERTY(QString owner READ getOwner)
    Q_PROPERTY(QString amount READ getAmount NOTIFY amountChanged)
    Q_PROPERTY(QList<QObject*> bills READ listBills NOTIFY billsChanged)

private:
    int mId;
    WalletType mType;
    QString mOwner;
    QString mCurrency;
    QString mAmount;
    QList<BillEntity*> mBills;

public:
    WalletEntity();
    WalletEntity(WalletType type, QString owner, QString currency);
    WalletEntity(QSqlRecord sqlRecord);
    ~WalletEntity();
    // Getters
    int getId();
    WalletType getType();
    QString getOwner();
    QString getCurrency();
    QString getAmount();
    QList<BillEntity*> getBills();
    // Setters
    void setType(WalletType type);
    void setOwner(QString owner);
    void setCurrency(QString currency);
    void setAmount(QString amount);
    void addBill(BillEntity* bill);
    void removeBill(BillEntity* bill);
    void addBillWad(QList<BillEntity*> wad);
    // Persitence
    void updateAfterPersist(int id);
    QString getSQLSelect(FetchMode fetchMode);
    QString getSQLInsert();
    QString getSQLUpdate(QString amount);
signals:
    void amountChanged();
    void billsChanged();
};

#endif // WALLETENTITY_H
