#ifndef WCPMESSAGECREATEMICROWALLETPACKAGEREQUEST_H
#define WCPMESSAGECREATEMICROWALLETPACKAGEREQUEST_H

#include "wcpmessage.h"
#include "bitcoinwallet.h"

class WCPMessageCreateMicroWalletPackageRequest : public WCPMessage
{
public:
    WCPMessageCreateMicroWalletPackageRequest();
    WCPMessageCreateMicroWalletPackageRequest(const WCPMessage &iOther);
    WCPMessageCreateMicroWalletPackageRequest(const QString iMessage);

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
    static QString chainTypeKey();

    int inputCount() const;
    int outputCount() const;
    BitcoinWallet::ChainType chainType() const;

    static QString create(
            const QString iCryptoTypeShortName, const QString iCryptoValue, const QString iTransactionId,
            const QString iUsername, const QByteArray iPrivateKey, WCPMessage::KeyEncoding iKeyEncoding = KeyEncoding::SHA512
            );

    static QString createBtc(const QString iCryptoValue, int iInputCount, int iOutputCount, const BitcoinWallet::ChainType iChainType,
            const QString iTransactionId,
            const QString iUsername, const QByteArray iPrivateKey, WCPMessage::KeyEncoding iKeyEncoding = KeyEncoding::SHA512
            );
};

#endif // WCPMESSAGECREATEMICROWALLETPACKAGEREQUEST_H
