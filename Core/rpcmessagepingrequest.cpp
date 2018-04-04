#include "rpcmessagepingrequest.h"

RPCMessagePingRequest::RPCMessagePingRequest() : RPCMessage()
{

}

RPCMessagePingRequest::RPCMessagePingRequest(const RPCMessage &iOther)
    : RPCMessage(iOther)
{

}

RPCMessagePingRequest::RPCMessagePingRequest(const QString iMessage)
    : RPCMessage(iMessage)
{

}

QString RPCMessagePingRequest::commandValue()
{ return QStringLiteral("ping"); }

QString RPCMessagePingRequest::pingPayloadKey()
{ return QStringLiteral("payload"); }

QByteArray RPCMessagePingRequest::payload() const
{ return fieldValue(pingPayloadKey()).toByteArray(); }
