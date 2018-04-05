#include "rpcmessagepingrequest.h"

RPCMessagePingRequest::RPCMessagePingRequest() : RPCMessage()
{ }

RPCMessagePingRequest::RPCMessagePingRequest(const RPCMessage &iOther)
    : RPCMessage(iOther)
{ }

RPCMessagePingRequest::RPCMessagePingRequest(const QString iMessage)
    : RPCMessage(iMessage)
{ }

QString RPCMessagePingRequest::commandValue()
{ return QStringLiteral("ping"); }

QString RPCMessagePingRequest::payloadKey()
{ return QStringLiteral("payload"); }

QByteArray RPCMessagePingRequest::payload() const
{ return fieldValue(payloadKey()).toByteArray(); }

QString RPCMessagePingRequest::create(const QByteArray iPayload, const QString iUsername)
{ return RPCMessage::toMessage( QList<RPCField>() << RPCField{payloadKey(), iPayload} << RPCField{QStringLiteral(kFieldKey_Command), commandValue()}, iUsername, "", RPCMessage::KeyEncoding::None ); }
