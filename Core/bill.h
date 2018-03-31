#ifndef BILL_H
#define BILL_H

#include <QObject>
#include <QString>
#include "wallet.h"

class Bill : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Wallet* wallet READ getWallet)
    Q_PROPERTY(QString address READ getAddress)
    Q_PROPERTY(QString publicKey READ getPublicKey)
    Q_PROPERTY(QString amount READ getAmount NOTIFY amountChanged)

private:
    Wallet* mWallet;
    QString mAddress;
    QString mPublicKey;
    QString mPrivateKey;
    QString mAmount;
    // Internal usage methods
    QString getPrivateKey();
    QString setAmount(QString amount);

public:
    Bill(Wallet* wallet);
    Wallet* getWallet();
    QString getAddress();
    QString getPublicKey();
    QString getAmount();

signals:
    void amountChanged();
};

#endif // BILL_H
