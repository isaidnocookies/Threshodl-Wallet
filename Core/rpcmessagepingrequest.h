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
    static QString payloadKey();

    QByteArray payload() const;

    static QString create(const QByteArray iPayload, const QString iUsername);
};

#endif // RPCMESSAGEPINGREQUEST_H
