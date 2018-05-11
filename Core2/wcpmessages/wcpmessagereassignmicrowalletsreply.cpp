#include "wcpmessagereassignmicrowalletsreply.h"

WCPMessageReassignMicroWalletsReply::WCPMessageReassignMicroWalletsReply() : WCPMessage()
{ }

WCPMessageReassignMicroWalletsReply::WCPMessageReassignMicroWalletsReply(const WCPMessage &iOther)
    : WCPMessage(iOther)
{ }

WCPMessageReassignMicroWalletsReply::WCPMessageReassignMicroWalletsReply(const QString iMessage)
    : WCPMessage(iMessage)
{ }

QString WCPMessageReassignMicroWalletsReply::commandValue()
{ return QStringLiteral("reassignMicroWallets"); }

QString WCPMessageReassignMicroWalletsReply::replyCodeKey()
{ return QStringLiteral("replyCode"); }

QString WCPMessageReassignMicroWalletsReply::transactionIdKey()
{ return QStringLiteral("transactionId"); }

WCPMessageReassignMicroWalletsReply::ReplyCode WCPMessageReassignMicroWalletsReply::replyCode() const
{ return static_cast<ReplyCode>(fieldValue(replyCodeKey()).toUInt()); }

QString WCPMessageReassignMicroWalletsReply::transactionId() const
{ return fieldValue(transactionIdKey()).toString(); }

QString WCPMessageReassignMicroWalletsReply::create(const WCPMessageReassignMicroWalletsReply::ReplyCode iReplyCode, const QString iTransactionId, const QString iUsername, const QByteArray iPrivateKey, WCPMessage::KeyEncoding iKeyEncoding)
{
    return WCPMessage::toMessage(
                QList<WCPField>()
                << WCPField{replyCodeKey(), static_cast<unsigned int>(iReplyCode)}
                << WCPField{transactionIdKey(), iTransactionId}
                << WCPField{QStringLiteral(kFieldKey_Command), commandValue()},
                iUsername, iPrivateKey, iKeyEncoding
                );
}
