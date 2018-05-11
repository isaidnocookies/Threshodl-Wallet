#include "wcpmessagereassignmicrowalletsrequest.h"

WCPMessageReassignMicroWalletsRequest::WCPMessageReassignMicroWalletsRequest() : WCPMessage()
{ }

WCPMessageReassignMicroWalletsRequest::WCPMessageReassignMicroWalletsRequest(const WCPMessage &iOther)
    : WCPMessage(iOther)
{ }

WCPMessageReassignMicroWalletsRequest::WCPMessageReassignMicroWalletsRequest(const QString iMessage)
    : WCPMessage(iMessage)
{ }

QString WCPMessageReassignMicroWalletsRequest::commandValue()
{ return QStringLiteral("reassignMicroWallets"); }

QString WCPMessageReassignMicroWalletsRequest::destinationKey()
{ return QStringLiteral("destination"); }

QString WCPMessageReassignMicroWalletsRequest::microWalletIdsKey()
{ return QStringLiteral("microWalletIds"); }

QString WCPMessageReassignMicroWalletsRequest::transactionIdKey()
{ return QStringLiteral("transactionId"); }

QString WCPMessageReassignMicroWalletsRequest::destination() const
{ return fieldValue(destinationKey()).toString(); }

QStringList WCPMessageReassignMicroWalletsRequest::microWalletIds() const
{ return fieldValue(microWalletIdsKey()).toStringList(); }

QString WCPMessageReassignMicroWalletsRequest::transactionId() const
{ return fieldValue(transactionIdKey()).toString(); }

QString WCPMessageReassignMicroWalletsRequest::create(const QString iDestination, const QStringList iMicroWalletIds, const QString iTransactionId, const QString iUsername, const QByteArray iPrivateKey, WCPMessage::KeyEncoding iKeyEncoding)
{
    return WCPMessage::toMessage(
                QList<WCPField>()
                << WCPField{destinationKey(), iDestination}
                << WCPField{microWalletIdsKey(), iMicroWalletIds}
                << WCPField{transactionIdKey(), iTransactionId}
                << WCPField{QStringLiteral(kFieldKey_Command), commandValue()},
                iUsername, iPrivateKey, iKeyEncoding
                );
}
