#ifndef RPCMESSAGECREATEACCOUNTREQUEST_H
#define RPCMESSAGECREATEACCOUNTREQUEST_H

#include "rpcmessage.h"

class RPCMessageCreateAccountRequest : public RPCMessage
{
public:
    RPCMessageCreateAccountRequest();
    RPCMessageCreateAccountRequest(const RPCMessage &iOther);
    RPCMessageCreateAccountRequest(const QString iMessage);

    static QString commandValue();
    static QString publicKeyKey();

    QByteArray publicKey() const;

    static QString create(const QByteArray iPublicKey, const QString iUsername, const QByteArray iPrivateKey, RPCMessage::KeyEncoding iKeyEncoding = KeyEncoding::SHA512);
    static QString createInternal(const QByteArray iPublicKey, const QString iUsername, const QByteArray iPrivateKey, RPCMessage::KeyEncoding iKeyEncoding = KeyEncoding::SHA512);
};

#endif // RPCMESSAGECREATEACCOUNTREQUEST_H
