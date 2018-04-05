#include "rpcmessagepingreply.h"

RPCMessagePingReply::RPCMessagePingReply() : RPCMessage()
{ }

RPCMessagePingReply::RPCMessagePingReply(const RPCMessage &iOther)
    : RPCMessage(iOther)
{ }

RPCMessagePingReply::RPCMessagePingReply(const QString iMessage)
    : RPCMessage(iMessage)
{ }

QString RPCMessagePingReply::commandValue()
{ return QStringLiteral("ping"); }

QString RPCMessagePingReply::pingPayloadKey()
{ return QStringLiteral("payload"); }

QByteArray RPCMessagePingReply::payload() const
{ return QByteArray::fromBase64(fieldValue(pingPayloadKey()).toByteArray()); }

QString RPCMessagePingReply::create(const QByteArray iPayload, const QString iUsername)
{ return RPCMessage::toMessage( QList<RPCField>() << RPCField{pingPayloadKey(), iPayload.toBase64()} << RPCField{QStringLiteral(kFieldKey_Command), commandValue()}, iUsername, "", RPCMessage::KeyEncoding::None ); }
