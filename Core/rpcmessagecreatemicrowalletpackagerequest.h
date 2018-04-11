#ifndef RPCMESSAGECREATEMICROWALLETPACKAGEREQUEST_H
#define RPCMESSAGECREATEMICROWALLETPACKAGEREQUEST_H

#include "rpcmessage.h"

class RPCMessageCreateMicroWalletPackageRequest : public RPCMessage
{
public:
    RPCMessageCreateMicroWalletPackageRequest();
    RPCMessageCreateMicroWalletPackageRequest(const RPCMessage &iOther);
    RPCMessageCreateMicroWalletPackageRequest(const QString iMessage);

    static QString commandValue();
    static QString cryptoTypeShortNameKey();
    static QString cryptoValueKey();

    QString cryptoTypeShortName() const;
    QString cryptoValue() const;

    // For Bitcoin
    static QString txIdKey();
    static QString voutKey();
    static QString outputBalanceAddressKey();
    static QString outputBalanceKey();
    QString txId() const;
    QString vout() const;
    QString outputBalanceAddress() const;
    QString outputBalance() const;

    static QString create(
            const QString iCryptoTypeShortName, const QString iCryptoValue,
            const QString iUsername, const QByteArray iPrivateKey, RPCMessage::KeyEncoding iKeyEncoding = KeyEncoding::SHA512
            );

    static QString createBtc(const QString iCryptoValue, const QString iTxId, const QString iVout,
            const QString iUsername, const QByteArray iPrivateKey, const QString iOutputBalanceAddress = QStringLiteral(""), const QString iOutputBalance = QStringLiteral("0.0"), RPCMessage::KeyEncoding iKeyEncoding = KeyEncoding::SHA512
            );
};

#endif // RPCMESSAGECREATEMICROWALLETPACKAGEREQUEST_H
