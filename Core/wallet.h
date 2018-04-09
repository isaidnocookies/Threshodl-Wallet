#ifndef WALLET_H
#define WALLET_H

#include <QString>
#include <QVariantMap>

class Wallet
{
protected:
    QString     mShortNameType;         // Example: BTC
    QString     mLongNameType;          // Example: Bitcoin
    QString     mValue;                 // Example: 0.12345678
    QByteArray  mPrivateKey;
    QByteArray  mPublicKey;
    QByteArray  mAddress;

    Wallet()
    { }

public:
    QByteArray _baseClassToData(QVariantMap iMap) const;
    void _baseClassFromData(QVariantMap iDataMap);

public:
    Wallet(const QByteArray iData);

    virtual QString     shortNameType() const   { return mShortNameType; }
    virtual QString     longNameType() const    { return mLongNameType; }
    virtual QString     value() const           { return mValue; }
    virtual QByteArray  privateKey() const      { return mPrivateKey; }
    virtual QByteArray  publicKey() const       { return mPublicKey; }
    virtual QByteArray  address() const         { return mAddress; }
    virtual QByteArray  toData() const          { return _baseClassToData(QVariantMap()); }
};

#endif // WALLET_H
