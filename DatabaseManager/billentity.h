#ifndef BILLENTITY_H
#define BILLENTITY_H

#include <QObject>
#include <QString>
#include <QSqlRecord>
#include "walletentity.h"
#include "entity.h"

class WalletEntity;

class BillEntity : public Entity, public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString address READ getAddress)
    Q_PROPERTY(QString publicKey READ getPublicKey)
    Q_PROPERTY(QString currency READ getCurrency)
    Q_PROPERTY(QString amount READ getAmount NOTIFY amountChanged)

private:
    int mId;
    WalletEntity* mWallet;
    QString mOwner;
    QString mAddress;
    QString mPublicKey;
    QString mPrivateKey;
    QString mCurrency;
    QString mAmount;

public:
    BillEntity(WalletEntity* wallet);
    BillEntity(QSqlRecord sqlrecord, WalletEntity* wallet);
    ~BillEntity();
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
signals:
    void amountChanged();
};

#endif // BILLENTITY_H
