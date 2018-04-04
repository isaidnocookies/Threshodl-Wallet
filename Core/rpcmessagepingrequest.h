#ifndef RPCMESSAGEPINGREQUEST_H
#define RPCMESSAGEPINGREQUEST_H

#include "rpcmessage.h"

class RPCMessagePingRequest : public RPCMessage
{
public:
    RPCMessagePingRequest();
    RPCMessagePingRequest(const RPCMessage &iOther);
    RPCMessagePingRequest(const QString iMessage);

    static QString commandValue();
    static QString pingPayloadKey();

    QByteArray payload() const;

    static QString create(const QByteArray iPayload, const QString iUsername, const QByteArray iPublicKey, RPCMessage::KeyEncoding iKeyEncoding = KeyEncoding::SHA512);
};

#endif // RPCMESSAGEPINGREQUEST_H
