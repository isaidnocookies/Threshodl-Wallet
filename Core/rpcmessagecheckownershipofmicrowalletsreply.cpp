#include "rpcmessagecheckownershipofmicrowalletsreply.h"

RPCMessageCheckOwnershipOfMicroWalletsReply::RPCMessageCheckOwnershipOfMicroWalletsReply() : RPCMessage()
{ }

RPCMessageCheckOwnershipOfMicroWalletsReply::RPCMessageCheckOwnershipOfMicroWalletsReply(const RPCMessage &iOther) : RPCMessage(iOther)
{ }

RPCMessageCheckOwnershipOfMicroWalletsReply::RPCMessageCheckOwnershipOfMicroWalletsReply(const QString iMessage) : RPCMessage(iMessage)
{ }

QString RPCMessageCheckOwnershipOfMicroWalletsReply::commandValue()
{ return QStringLiteral("checkOwnershipOfMicroWallets"); }

QString RPCMessageCheckOwnershipOfMicroWalletsReply::transactionIdKey()
{ return QStringLiteral("transactionId"); }

QString RPCMessageCheckOwnershipOfMicroWalletsReply::replyCodeKey()
{ return QStringLiteral("replyCode"); }

RPCMessageCheckOwnershipOfMicroWalletsReply::ReplyCode RPCMessageCheckOwnershipOfMicroWalletsReply::replyCode() const
{ return static_cast<ReplyCode>(fieldValue(replyCodeKey()).toUInt()); }

QString RPCMessageCheckOwnershipOfMicroWalletsReply::transactionId() const
{ return fieldValue(transactionIdKey()).toString(); }

QString RPCMessageCheckOwnershipOfMicroWalletsReply::create(const RPCMessageCheckOwnershipOfMicroWalletsReply::ReplyCode iReplyCode, const QString iTransactionId, const QString iUsername, const QByteArray iPrivateKey, RPCMessage::KeyEncoding iKeyEncoding)
{
    return RPCMessage::toMessage(
                QList<RPCField>()
                << RPCField{replyCodeKey(), static_cast<unsigned int>(iReplyCode)}
                << RPCField{transactionIdKey(), iTransactionId}
                << RPCField{QStringLiteral(kFieldKey_Command), commandValue()},
                iUsername, iPrivateKey, iKeyEncoding
                );
}
