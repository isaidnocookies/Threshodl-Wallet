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

QString RPCMessagePingRequest::create(const QByteArray iPayload, const QString iUsername, const QByteArray iPublicKey, RPCMessage::KeyEncoding iKeyEncoding)
{ return RPCMessage::toMessage( QList<RPCField>() << RPCField{pingPayloadKey(), iPayload} << RPCField{QStringLiteral(kFieldKey_Command), commandValue()}, iUsername, iPublicKey, iKeyEncoding ); }
