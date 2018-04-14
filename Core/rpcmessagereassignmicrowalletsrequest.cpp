#include "rpcmessagereassignmicrowalletsrequest.h"

RPCMessageReassignMicroWalletsRequest::RPCMessageReassignMicroWalletsRequest() : RPCMessage()
{ }

RPCMessageReassignMicroWalletsRequest::RPCMessageReassignMicroWalletsRequest(const RPCMessage &iOther)
    : RPCMessage(iOther)
{ }

RPCMessageReassignMicroWalletsRequest::RPCMessageReassignMicroWalletsRequest(const QString iMessage)
    : RPCMessage(iMessage)
{ }

QString RPCMessageReassignMicroWalletsRequest::commandValue()
{ return QStringLiteral("reassignMicroWallets"); }

QString RPCMessageReassignMicroWalletsRequest::destinationKey()
{ return QStringLiteral("destination"); }

QString RPCMessageReassignMicroWalletsRequest::microWalletIdsKey()
{ return QStringLiteral("microWalletIds"); }

QString RPCMessageReassignMicroWalletsRequest::transactionIdKey()
{ return QStringLiteral("transactionId"); }

QString RPCMessageReassignMicroWalletsRequest::destination() const
{ return fieldValue(destinationKey()).toString(); }

QStringList RPCMessageReassignMicroWalletsRequest::microWalletIds() const
{ return fieldValue(microWalletIdsKey()).toStringList(); }

QString RPCMessageReassignMicroWalletsRequest::transactionId() const
{ return fieldValue(transactionIdKey()).toString(); }

QString RPCMessageReassignMicroWalletsRequest::create(const QString iDestination, const QStringList iMicroWalletIds, const QString iTransactionId, const QString iUsername, const QByteArray iPrivateKey, RPCMessage::KeyEncoding iKeyEncoding)
{
    return RPCMessage::toMessage(
                QList<RPCField>()
                << RPCField{destinationKey(), iDestination}
                << RPCField{microWalletIdsKey(), iMicroWalletIds}
                << RPCField{transactionIdKey(), iTransactionId}
                << RPCField{QStringLiteral(kFieldKey_Command), commandValue()},
                iUsername, iPrivateKey, iKeyEncoding
                );
}
