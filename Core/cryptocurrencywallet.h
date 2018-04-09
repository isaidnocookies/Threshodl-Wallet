#ifndef CRYPTOCURRENCYWALLET_H
#define CRYPTOCURRENCYWALLET_H

#include "wallet.h"
#include "microwallet.h"

class CryptoCurrencyWallet : public Wallet, public MicroWallet
{
public:
    CryptoCurrencyWallet() = default;
    virtual ~CryptoCurrencyWallet() = default;
};

#endif // CRYPTOCURRENCYWALLET_H
