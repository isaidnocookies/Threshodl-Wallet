#ifndef BILL_H
#define BILL_H

#include <QObject>
#include <QString>
#include "wallet.h"

// Forward declaration to deal with class interdependency
class Wallet;

class Bill : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString address READ getAddress)
    Q_PROPERTY(QString publicKey READ getPublicKey)
    Q_PROPERTY(QString currency READ getCurrency)
    Q_PROPERTY(QString amount READ getAmount NOTIFY amountChanged)

private:
    Wallet* mWallet;
    QString mAddress;
    QString mPublicKey;
    QString mPrivateKey;
    QString mCurrency;
    QString mAmount;
    // Internal usage methods
    QString getPrivateKey();
    QString setAmount(QString amount);

public:
    Bill(Wallet* wallet);
    Bill(Wallet* wallet, QString address, QString publickey, QString privateKey, QString currency, QString amount);
    Wallet* getWallet();
    QString getAddress();
    QString getPublicKey();
    QString getCurrency();
    QString getAmount();

signals:
    void amountChanged();
};

#endif // BILL_H
