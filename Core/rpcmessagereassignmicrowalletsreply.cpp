#include "rpcmessagereassignmicrowalletsreply.h"

RPCMessageReassignMicroWalletsReply::RPCMessageReassignMicroWalletsReply() : RPCMessage()
{ }

RPCMessageReassignMicroWalletsReply::RPCMessageReassignMicroWalletsReply(const RPCMessage &iOther)
    : RPCMessage(iOther)
{ }

RPCMessageReassignMicroWalletsReply::RPCMessageReassignMicroWalletsReply(const QString iMessage)
    : RPCMessage(iMessage)
{ }

QString RPCMessageReassignMicroWalletsReply::commandValue()
{ return QStringLiteral("reassignMicroWallets"); }

QString RPCMessageReassignMicroWalletsReply::replyCodeKey()
{ return QStringLiteral("replyCode"); }

QString RPCMessageReassignMicroWalletsReply::transactionIdKey()
{ return QStringLiteral("transactionId"); }

RPCMessageReassignMicroWalletsReply::ReplyCode RPCMessageReassignMicroWalletsReply::replyCode() const
{ return static_cast<ReplyCode>(fieldValue(replyCodeKey()).toUInt()); }

QString RPCMessageReassignMicroWalletsReply::transactionId() const
{ return fieldValue(transactionId()).toString(); }

QString RPCMessageReassignMicroWalletsReply::create(const RPCMessageReassignMicroWalletsReply::ReplyCode iReplyCode, const QString iTransactionId, const QString iUsername, const QByteArray iPrivateKey, RPCMessage::KeyEncoding iKeyEncoding)
{
    return RPCMessage::toMessage(
                QList<RPCField>()
                << RPCField{replyCodeKey(), static_cast<unsigned int>(iReplyCode)}
                << RPCField{transactionIdKey(), iTransactionId}
                << RPCField{QStringLiteral(kFieldKey_Command), commandValue()},
                iUsername, iPrivateKey, iKeyEncoding
                );
}
