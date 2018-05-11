#include "wcpmessagecompletemicrowalletsrequest.h"

WCPMessageCompleteMicroWalletsRequest::WCPMessageCompleteMicroWalletsRequest() : WCPMessage()
{ }

WCPMessageCompleteMicroWalletsRequest::WCPMessageCompleteMicroWalletsRequest(const WCPMessage &iOther) : WCPMessage(iOther)
{ }

WCPMessageCompleteMicroWalletsRequest::WCPMessageCompleteMicroWalletsRequest(const QString iMessage) : WCPMessage(iMessage)
{ }

QString WCPMessageCompleteMicroWalletsRequest::commandValue()
{ return QStringLiteral("completeMicroWallets"); }

QString WCPMessageCompleteMicroWalletsRequest::transactionIdKey()
{ return QStringLiteral("transactionId"); }

QString WCPMessageCompleteMicroWalletsRequest::walletIdsKey()
{ return QStringLiteral("walletIds"); }

QString WCPMessageCompleteMicroWalletsRequest::transactionId() const
{ return fieldValue(transactionIdKey()).toString(); }

QStringList WCPMessageCompleteMicroWalletsRequest::walletIds() const
{ return fieldValue(walletIdsKey()).toStringList(); }

QString WCPMessageCompleteMicroWalletsRequest::create(const QStringList iWalletIds, const QString iTransactionId, const QString iUsername, const QByteArray iPrivateKey, WCPMessage::KeyEncoding iKeyEncoding)
{
    return WCPMessage::toMessage(
                QList<WCPField>()
                << WCPField{walletIdsKey(), iWalletIds}
                << WCPField{transactionIdKey(), iTransactionId}
                << WCPField{QStringLiteral(kFieldKey_Command), commandValue()},
                iUsername, iPrivateKey, iKeyEncoding
                );
}
