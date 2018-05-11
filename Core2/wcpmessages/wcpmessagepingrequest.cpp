#include "wcpmessagepingrequest.h"

WCPMessagePingRequest::WCPMessagePingRequest() : WCPMessage()
{ }

WCPMessagePingRequest::WCPMessagePingRequest(const WCPMessage &iOther)
    : WCPMessage(iOther)
{ }

WCPMessagePingRequest::WCPMessagePingRequest(const QString iMessage)
    : WCPMessage(iMessage)
{ }

QString WCPMessagePingRequest::commandValue()
{ return QStringLiteral("ping"); }

QString WCPMessagePingRequest::payloadKey()
{ return QStringLiteral("payload"); }

QByteArray WCPMessagePingRequest::payload() const
{ return QByteArray::fromBase64(fieldValue(payloadKey()).toByteArray()); }

QString WCPMessagePingRequest::create(const QByteArray iPayload, const QString iUsername)
{ return WCPMessage::toMessage( QList<WCPField>() << WCPField{payloadKey(), iPayload.toBase64()} << WCPField{QStringLiteral(kFieldKey_Command), commandValue()}, iUsername, "", WCPMessage::KeyEncoding::None ); }
