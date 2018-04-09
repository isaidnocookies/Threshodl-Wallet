#ifndef BITCOINMICROWALLET_H
#define BITCOINMICROWALLET_H

#include "microwallet.h"
#include "bitcoinwallet.h"

class BitcoinMicroWallet : public MicroWallet, public BitcoinWallet
{
protected:
    BitcoinMicroWallet() : MicroWallet(), BitcoinWallet()
    { }

public:
    BitcoinMicroWallet(const QByteArray iData);

    virtual ~BitcoinMicroWallet();

    virtual QByteArray  toData() const override;
};

#endif // BITCOINMICROWALLET_H
