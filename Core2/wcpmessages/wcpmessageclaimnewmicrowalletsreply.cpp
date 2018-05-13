#include "wcpmessageclaimnewmicrowalletsreply.h"

WCPMessageClaimNewMicroWalletsReply::WCPMessageClaimNewMicroWalletsReply() : WCPMessage()
{ }

WCPMessageClaimNewMicroWalletsReply::WCPMessageClaimNewMicroWalletsReply(const WCPMessage &iOther) : WCPMessage(iOther)
{ }

WCPMessageClaimNewMicroWalletsReply::WCPMessageClaimNewMicroWalletsReply(const QString iMessage) : WCPMessage(iMessage)
{ }

QString WCPMessageClaimNewMicroWalletsReply::commandValue()
{ return QStringLiteral("claimNewMicroWallets"); }

QString WCPMessageClaimNewMicroWalletsReply::replyCodeKey()
{ return QStringLiteral("replyCode"); }

QString WCPMessageClaimNewMicroWalletsReply::transactionIdKey()
{ return QStringLiteral("transactionId"); }

QString WCPMessageClaimNewMicroWalletsReply::claimedWalletIdsKey()
{ return QStringLiteral("claimedWalletIds"); }

WCPMessageClaimNewMicroWalletsReply::ReplyCode WCPMessageClaimNewMicroWalletsReply::replyCode() const
{ return static_cast<ReplyCode>(fieldValue(replyCodeKey()).toUInt()); }

QString WCPMessageClaimNewMicroWalletsReply::transactionId() const
{ return fieldValue(transactionIdKey()).toString(); }

QStringList WCPMessageClaimNewMicroWalletsReply::claimedWalletIds() const
{ return fieldValue(claimedWalletIdsKey()).toStringList(); }

QString WCPMessageClaimNewMicroWalletsReply::create(const WCPMessageClaimNewMicroWalletsReply::ReplyCode iReplyCode, const QStringList iClaimedWalletIds, const QString iTransactionId, const QString iUsername, const QByteArray iPrivateKey, WCPMessage::KeyEncoding iKeyEncoding)
{
    return WCPMessage::toMessage(
                QList<WCPField>()
                << WCPField{replyCodeKey(), static_cast<unsigned int>(iReplyCode)}
                << WCPField{claimedWalletIdsKey(), iClaimedWalletIds}
                << WCPField{transactionIdKey(), iTransactionId}
                << WCPField{QStringLiteral(kFieldKey_Command), commandValue()},
                iUsername, iPrivateKey, iKeyEncoding
                );
}
