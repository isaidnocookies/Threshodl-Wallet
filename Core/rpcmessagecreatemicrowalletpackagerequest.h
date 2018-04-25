#ifndef RPCMESSAGECREATEMICROWALLETPACKAGEREQUEST_H
#define RPCMESSAGECREATEMICROWALLETPACKAGEREQUEST_H

#include "rpcmessage.h"
#include "bitcoinwallet.h"

class RPCMessageCreateMicroWalletPackageRequest : public RPCMessage
{
public:
    RPCMessageCreateMicroWalletPackageRequest();
    RPCMessageCreateMicroWalletPackageRequest(const RPCMessage &iOther);
    RPCMessageCreateMicroWalletPackageRequest(const QString iMessage);

    static QString commandValue();
    static QString cryptoTypeShortNameKey();
    static QString cryptoValueKey();
    static QString transactionIdKey();              // Untouched by the server

    QString cryptoTypeShortName() const;
    QString cryptoValue() const;
    QString transactionId() const;

    // For Bitcoin
    static QString inputCountKey();
    static QString outputCountKey();
    static QString outputBalanceAddressKey();
    static QString outputBalanceKey();
    static QString chainTypeKey();

    int inputCount() const;
    int outputCount() const;
    QString outputBalanceAddress() const;
    QString outputBalance() const;
    BitcoinWallet::ChainType chainType() const;

    static QString create(
            const QString iCryptoTypeShortName, const QString iCryptoValue, const QString iTransactionId,
            const QString iUsername, const QByteArray iPrivateKey, RPCMessage::KeyEncoding iKeyEncoding = KeyEncoding::SHA512
            );

    static QString createBtc(const QString iCryptoValue, int iInputCount, int iOutputCount, const BitcoinWallet::ChainType iChainType,
            const QString iTransactionId,
            const QString iUsername, const QByteArray iPrivateKey, const QString iOutputBalanceAddress = QStringLiteral(""), const QString iOutputBalance = QStringLiteral("0.0"), RPCMessage::KeyEncoding iKeyEncoding = KeyEncoding::SHA512
            );
};

#endif // RPCMESSAGECREATEMICROWALLETPACKAGEREQUEST_H
