#ifndef WALLETENTITY_H
#define WALLETENTITY_H

#include "walletentity.h"
#include "entity.h"
#include <QString>
#include <QSqlRecord>
#include <QList>
#include "billentity.h"

class BillEntity;

class WalletEntity : public Entity
{
private:
    int id;
    WalletType type;
    QString owner;
    QString currency;
    QString amount;
    QList<BillEntity*> bills;

public:
    WalletEntity();
    WalletEntity(RepositoryType repositoryType);
    WalletEntity(WalletType type, QString owner, QString currency);
    WalletEntity(WalletType type, QString owner, QString currency, RepositoryType repositoryType);
    WalletEntity(QSqlRecord sqlRecord);
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
};

#endif // WALLETENTITY_H
