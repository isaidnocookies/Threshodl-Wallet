#include "wcpmessagecheckownershipofmicrowalletsreply.h"

WCPMessageCheckOwnershipOfMicroWalletsReply::WCPMessageCheckOwnershipOfMicroWalletsReply() : WCPMessage()
{ }

WCPMessageCheckOwnershipOfMicroWalletsReply::WCPMessageCheckOwnershipOfMicroWalletsReply(const WCPMessage &iOther) : WCPMessage(iOther)
{ }

WCPMessageCheckOwnershipOfMicroWalletsReply::WCPMessageCheckOwnershipOfMicroWalletsReply(const QString iMessage) : WCPMessage(iMessage)
{ }

QString WCPMessageCheckOwnershipOfMicroWalletsReply::commandValue()
{ return QStringLiteral("checkOwnershipOfMicroWallets"); }

QString WCPMessageCheckOwnershipOfMicroWalletsReply::transactionIdKey()
{ return QStringLiteral("transactionId"); }

QString WCPMessageCheckOwnershipOfMicroWalletsReply::replyCodeKey()
{ return QStringLiteral("replyCode"); }

WCPMessageCheckOwnershipOfMicroWalletsReply::ReplyCode WCPMessageCheckOwnershipOfMicroWalletsReply::replyCode() const
{ return static_cast<ReplyCode>(fieldValue(replyCodeKey()).toUInt()); }

QString WCPMessageCheckOwnershipOfMicroWalletsReply::transactionId() const
{ return fieldValue(transactionIdKey()).toString(); }

QString WCPMessageCheckOwnershipOfMicroWalletsReply::create(const WCPMessageCheckOwnershipOfMicroWalletsReply::ReplyCode iReplyCode, const QString iTransactionId, const QString iUsername, const QByteArray iPrivateKey, WCPMessage::KeyEncoding iKeyEncoding)
{
    return WCPMessage::toMessage(
                QList<WCPField>()
                << WCPField{replyCodeKey(), static_cast<unsigned int>(iReplyCode)}
                << WCPField{transactionIdKey(), iTransactionId}
                << WCPField{QStringLiteral(kFieldKey_Command), commandValue()},
                iUsername, iPrivateKey, iKeyEncoding
                );
}
