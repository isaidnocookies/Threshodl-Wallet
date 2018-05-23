#include "wcpmessagesendnoderestmessagerequest.h"

WCPMessageSendNodeRestMessageRequest::WCPMessageSendNodeRestMessageRequest() : WCPMessage()
{ }

WCPMessageSendNodeRestMessageRequest::WCPMessageSendNodeRestMessageRequest(const WCPMessage &iOther)
    : WCPMessage(iOther)
{ }

WCPMessageSendNodeRestMessageRequest::WCPMessageSendNodeRestMessageRequest(const QString iMessage)
    : WCPMessage(iMessage)
{ }

QString WCPMessageSendNodeRestMessageRequest::commandValue()
{ return QStringLiteral("sendNodeRestMessage"); }

QString WCPMessageSendNodeRestMessageRequest::cryptoTypeShortNameKey()
{ return QStringLiteral("cryptoTypeShortName"); }

QString WCPMessageSendNodeRestMessageRequest::transactionIdKey()
{ return QStringLiteral("transactionId"); }

QString WCPMessageSendNodeRestMessageRequest::urlPathKey()
{ return QStringLiteral("urlPath"); }

QString WCPMessageSendNodeRestMessageRequest::methodKey()
{ return QStringLiteral("method"); }

QString WCPMessageSendNodeRestMessageRequest::payloadTypeKey()
{ return QStringLiteral("payloadType"); }

QString WCPMessageSendNodeRestMessageRequest::payloadKey()
{ return QStringLiteral("payload"); }

QString WCPMessageSendNodeRestMessageRequest::cryptoTypeShortName() const
{ return fieldValue(cryptoTypeShortNameKey()).toString(); }

QString WCPMessageSendNodeRestMessageRequest::transactionId() const
{ return fieldValue(transactionIdKey()).toString(); }

QString WCPMessageSendNodeRestMessageRequest::urlPath() const
{ return fieldValue(urlPathKey()).toString(); }

WCPMessageSendNodeRestMessageRequest::Method WCPMessageSendNodeRestMessageRequest::method() const
{ return static_cast<Method>(fieldValue(methodKey()).toUInt()); }

QString WCPMessageSendNodeRestMessageRequest::payloadType() const
{ return fieldValue(payloadTypeKey()).toString(); }

QByteArray WCPMessageSendNodeRestMessageRequest::payload() const
{ return QByteArray::fromBase64(fieldValue(payloadKey()).toByteArray()); }

QString WCPMessageSendNodeRestMessageRequest::createGet(const QString iCryptoTypeShortName, const QString iUrlPath, const QString iTransactionId, const QString iUsername, const QByteArray iPrivateKey, WCPMessage::KeyEncoding iKeyEncoding)
{
    return WCPMessage::toMessage(
                QList<WCPField>()
                << WCPField{cryptoTypeShortNameKey(), iCryptoTypeShortName}
                << WCPField{urlPathKey(), iUrlPath}
                << WCPField{transactionIdKey(), iTransactionId}
                << WCPField{methodKey(), static_cast<unsigned int>(Method::Get)}
                << WCPField{QStringLiteral(kFieldKey_Command), commandValue()},
                iUsername, iPrivateKey, iKeyEncoding
                );
}

QString WCPMessageSendNodeRestMessageRequest::createPost(const QString iCryptoTypeShortName, const QString iUrlPath, const QString iTransactionId, const QString iPayloadType, const QByteArray iPayload, const QString iUsername, const QByteArray iPrivateKey, WCPMessage::KeyEncoding iKeyEncoding)
{
    return WCPMessage::toMessage(
                QList<WCPField>()
                << WCPField{cryptoTypeShortNameKey(), iCryptoTypeShortName}
                << WCPField{urlPathKey(), iUrlPath}
                << WCPField{transactionIdKey(), iTransactionId}
                << WCPField{methodKey(), static_cast<unsigned int>(Method::Post)}
                << WCPField{payloadTypeKey(), iPayloadType}
                << WCPField{payloadKey(), iPayload.toBase64()}
                << WCPField{QStringLiteral(kFieldKey_Command), commandValue()},
                iUsername, iPrivateKey, iKeyEncoding
                );
}
