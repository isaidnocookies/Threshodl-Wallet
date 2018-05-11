#include "wcpmessagecheckownershipofmicrowalletsrequest.h"

WCPMessageCheckOwnershipOfMicroWalletsRequest::WCPMessageCheckOwnershipOfMicroWalletsRequest() : WCPMessage()
{ }

WCPMessageCheckOwnershipOfMicroWalletsRequest::WCPMessageCheckOwnershipOfMicroWalletsRequest(const WCPMessage &iOther) : WCPMessage(iOther)
{ }

WCPMessageCheckOwnershipOfMicroWalletsRequest::WCPMessageCheckOwnershipOfMicroWalletsRequest(const QString iMessage) : WCPMessage(iMessage)
{ }

QString WCPMessageCheckOwnershipOfMicroWalletsRequest::commandValue()
{ return QStringLiteral("checkOwnershipOfMicroWallets"); }

QString WCPMessageCheckOwnershipOfMicroWalletsRequest::transactionIdKey()
{ return QStringLiteral("transactionId"); }

QString WCPMessageCheckOwnershipOfMicroWalletsRequest::walletIdsKey()
{ return QStringLiteral("walletIds"); }

QString WCPMessageCheckOwnershipOfMicroWalletsRequest::ownerKey()
{ return QStringLiteral("owner"); }

QString WCPMessageCheckOwnershipOfMicroWalletsRequest::transactionId() const
{ return fieldValue(transactionIdKey()).toString(); }

QStringList WCPMessageCheckOwnershipOfMicroWalletsRequest::walletIds() const
{ return fieldValue(walletIdsKey()).toStringList(); }

QString WCPMessageCheckOwnershipOfMicroWalletsRequest::owner() const
{ return fieldValue(ownerKey()).toString(); }

QString WCPMessageCheckOwnershipOfMicroWalletsRequest::create(const QString iOwner, const QStringList iWalletIds, const QString iTransactionId, const QString iUsername, const QByteArray iPrivateKey, WCPMessage::KeyEncoding iKeyEncoding)
{
    return WCPMessage::toMessage(
                QList<WCPField>()
                << WCPField{ownerKey(), iOwner}
                << WCPField{walletIdsKey(), iWalletIds}
                << WCPField{transactionIdKey(), iTransactionId}
                << WCPField{QStringLiteral(kFieldKey_Command), commandValue()},
                iUsername, iPrivateKey, iKeyEncoding
                );
}
