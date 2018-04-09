#ifndef MICROWALLET_H
#define MICROWALLET_H

#include "wallet.h"

class MicroWallet
{
protected:
    bool            mPrivateKeyComplete = false;
    QByteArray      mMicroWalletId;

    QVariantMap _microWalletClassToData(QVariantMap iMap) const;
    void _microWalletClassFromData(QVariantMap iDataMap);

    MicroWallet() // : Wallet()
    { }

public:
//    MicroWallet(const QByteArray iData);

    virtual void        setPrivateKeyComplete(bool iComplete = false);
    virtual bool        privateKeyComplete() const      { return mPrivateKeyComplete; }

    virtual void        setMicroWalletId(const QByteArray iMicroWalletId);  // Should Only be done at the server
    virtual QByteArray  microWalletId() const;

//    virtual bool        completePrivateKey(const QByteArray iNewPrivateKey);
//    virtual QByteArray  toData() const override;
};

#endif // MICROWALLET_H
