#ifndef BILLENTITY_H
#define BILLENTITY_H

#include <QString>
#include <QSqlRecord>
#include "walletentity.h"
#include "entity.h"

class WalletEntity;

class BillEntity : public Entity
{
private:
    int id;
    WalletEntity* wallet;
    QString owner;
    QString address;
    QString publicKey;
    QString privateKey;
    QString currency;
    QString amount;

public:
    BillEntity(WalletEntity* wallet);
    BillEntity(WalletEntity* wallet, RepositoryType repositoryType);
    BillEntity(QSqlRecord sqlrecord, WalletEntity* wallet);
    // Getters
    int getId();
    WalletEntity* getWallet();
    QString getOwner();
    QString getAddress();
    QString getPublicKey();
    QString getPrivateKey();
    QString getCurrency();
    QString getAmount();
    // Setters
    void setWallet(WalletEntity* wallet);
    void setOwner(QString owner);
    void setAddress(QString address);
    void setPublicKey(QString publicKey);
    void setPrivateKey(QString privateKey);
    void setCurrency(QString currency);
    void setAmount(QString amount);
    // Persitence
    void updateAfterPersist(int id);
    QString getSQLSelect(FetchMode fetchMode);
    QString getSQLInsert();
    QString getSQLUpdate(QString amount);
};

#endif // BILLENTITY_H
