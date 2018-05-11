#include "wcpmessagepingreply.h"

WCPMessagePingReply::WCPMessagePingReply() : WCPMessage()
{ }

WCPMessagePingReply::WCPMessagePingReply(const WCPMessage &iOther)
    : WCPMessage(iOther)
{ }

WCPMessagePingReply::WCPMessagePingReply(const QString iMessage)
    : WCPMessage(iMessage)
{ }

QString WCPMessagePingReply::commandValue()
{ return QStringLiteral("ping"); }

QString WCPMessagePingReply::pingPayloadKey()
{ return QStringLiteral("payload"); }

QByteArray WCPMessagePingReply::payload() const
{ return QByteArray::fromBase64(fieldValue(pingPayloadKey()).toByteArray()); }

QString WCPMessagePingReply::create(const QByteArray iPayload, const QString iUsername)
{ return WCPMessage::toMessage( QList<WCPField>() << WCPField{pingPayloadKey(), iPayload.toBase64()} << WCPField{QStringLiteral(kFieldKey_Command), commandValue()}, iUsername, "", WCPMessage::KeyEncoding::None ); }
