#include "wcpmessageclaimnewmicrowalletsrequest.h"

WCPMessageClaimNewMicroWalletsRequest::WCPMessageClaimNewMicroWalletsRequest() : WCPMessage()
{ }

WCPMessageClaimNewMicroWalletsRequest::WCPMessageClaimNewMicroWalletsRequest(const WCPMessage &iOther) : WCPMessage(iOther)
{ }

WCPMessageClaimNewMicroWalletsRequest::WCPMessageClaimNewMicroWalletsRequest(const QString iMessage) : WCPMessage(iMessage)
{ }

QString WCPMessageClaimNewMicroWalletsRequest::commandValue()
{ return QStringLiteral("claimNewMicroWallets"); }

QString WCPMessageClaimNewMicroWalletsRequest::transactionIdKey()
{ return QStringLiteral("transactionId"); }

QString WCPMessageClaimNewMicroWalletsRequest::walletIdsKey()
{ return QStringLiteral("walletIds"); }

QString WCPMessageClaimNewMicroWalletsRequest::transactionId() const
{ return fieldValue(transactionIdKey()).toString(); }

QStringList WCPMessageClaimNewMicroWalletsRequest::walletIds() const
{ return fieldValue(walletIdsKey()).toStringList(); }

QString WCPMessageClaimNewMicroWalletsRequest::create(const QStringList iWalletIds, const QString iTransactionId, const QString iUsername, const QByteArray iPrivateKey, WCPMessage::KeyEncoding iKeyEncoding)
{
    return WCPMessage::toMessage(
                QList<WCPField>()
                << WCPField{walletIdsKey(), iWalletIds}
                << WCPField{transactionIdKey(), iTransactionId}
                << WCPField{QStringLiteral(kFieldKey_Command), commandValue()},
                iUsername, iPrivateKey, iKeyEncoding
                );
}
