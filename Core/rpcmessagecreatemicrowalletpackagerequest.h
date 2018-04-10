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

    static QString create(
            const QString iCryptoTypeShortName, const QString iCryptoValue,
            const QString iUsername, const QByteArray iPrivateKey, RPCMessage::KeyEncoding iKeyEncoding = KeyEncoding::SHA512
            );
};

#endif // RPCMESSAGECREATEMICROWALLETPACKAGEREQUEST_H
