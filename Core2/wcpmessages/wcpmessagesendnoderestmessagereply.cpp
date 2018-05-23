#include "wcpmessagesendnoderestmessagereply.h"

WCPMessageSendNodeRestMessageReply::WCPMessageSendNodeRestMessageReply() : WCPMessage()
{ }

WCPMessageSendNodeRestMessageReply::WCPMessageSendNodeRestMessageReply(const WCPMessage &iOther)
    : WCPMessage(iOther)
{ }

WCPMessageSendNodeRestMessageReply::WCPMessageSendNodeRestMessageReply(const QString iMessage)
    : WCPMessage(iMessage)
{ }

QString WCPMessageSendNodeRestMessageReply::commandValue()
{ return QStringLiteral("sendNodeRestMessage"); }

QString WCPMessageSendNodeRestMessageReply::transactionIdKey()
{ return QStringLiteral("transactionId"); }

QString WCPMessageSendNodeRestMessageReply::replyCodeKey()
{ return QStringLiteral("replyCode"); }

QString WCPMessageSendNodeRestMessageReply::payloadKey()
{ return QStringLiteral("payload"); }

QString WCPMessageSendNodeRestMessageReply::transactionId() const
{ return fieldValue(transactionIdKey()).toString(); }

QNetworkReply::NetworkError WCPMessageSendNodeRestMessageReply::replyCode() const
{ return static_cast<QNetworkReply::NetworkError>(fieldValue(replyCodeKey()).toUInt()); }

QByteArray WCPMessageSendNodeRestMessageReply::payload() const
{ return QByteArray::fromBase64(fieldValue(payloadKey()).toByteArray()); }

QString WCPMessageSendNodeRestMessageReply::create(const QString iTransactionId, const QByteArray iPayload, const QNetworkReply::NetworkError iReplyCode, const QString iUsername, const QByteArray iPrivateKey, WCPMessage::KeyEncoding iKeyEncoding)
{
    return WCPMessage::toMessage(
                QList<WCPField>()
                << WCPField{transactionIdKey(), iTransactionId}
                << WCPField{replyCodeKey(), static_cast<unsigned int>(iReplyCode)}
                << WCPField{payloadKey(), iPayload.toBase64()}
                << WCPField{QStringLiteral(kFieldKey_Command), commandValue()},
                iUsername, iPrivateKey, iKeyEncoding
                );
}
