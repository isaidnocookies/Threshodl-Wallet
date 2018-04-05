#ifndef RPCMESSAGEPINGREPLY_H
#define RPCMESSAGEPINGREPLY_H

#include "rpcmessage.h"

class RPCMessagePingReply : public RPCMessage
{
public:
    RPCMessagePingReply();
    RPCMessagePingReply(const RPCMessage &iOther);
    RPCMessagePingReply(const QString iMessage);

    static QString commandValue();
    static QString pingPayloadKey();

    QByteArray payload() const;

    static QString create(const QByteArray iPayload, const QString iUsername, const QByteArray iPrivateKey, RPCMessage::KeyEncoding iKeyEncoding = KeyEncoding::SHA512);
};

#endif // RPCMESSAGEPINGREPLY_H
