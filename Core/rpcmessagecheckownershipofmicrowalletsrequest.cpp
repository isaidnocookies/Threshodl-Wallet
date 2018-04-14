#include "rpcmessagecheckownershipofmicrowalletsrequest.h"

RPCMessageCheckOwnershipOfMicroWalletsRequest::RPCMessageCheckOwnershipOfMicroWalletsRequest() : RPCMessage()
{ }

RPCMessageCheckOwnershipOfMicroWalletsRequest::RPCMessageCheckOwnershipOfMicroWalletsRequest(const RPCMessage &iOther) : RPCMessage(iOther)
{ }

RPCMessageCheckOwnershipOfMicroWalletsRequest::RPCMessageCheckOwnershipOfMicroWalletsRequest(const QString iMessage) : RPCMessage(iMessage)
{ }

QString RPCMessageCheckOwnershipOfMicroWalletsRequest::commandValue()
{ return QStringLiteral("checkOwnershipOfMicroWallets"); }

QString RPCMessageCheckOwnershipOfMicroWalletsRequest::transactionIdKey()
{ return QStringLiteral("transactionId"); }

QString RPCMessageCheckOwnershipOfMicroWalletsRequest::walletIdsKey()
{ return QStringLiteral("walletIds"); }

QString RPCMessageCheckOwnershipOfMicroWalletsRequest::ownerKey()
{ return QStringLiteral("owner"); }

QString RPCMessageCheckOwnershipOfMicroWalletsRequest::transactionId() const
{ return fieldValue(transactionIdKey()).toString(); }

QStringList RPCMessageCheckOwnershipOfMicroWalletsRequest::walletIds() const
{ return fieldValue(walletIdsKey()).toStringList(); }

QString RPCMessageCheckOwnershipOfMicroWalletsRequest::owner() const
{ return fieldValue(ownerKey()).toString(); }

QString RPCMessageCheckOwnershipOfMicroWalletsRequest::create(const QString iOwner, const QStringList iWalletIds, const QString iTransactionId, const QString iUsername, const QByteArray iPrivateKey, RPCMessage::KeyEncoding iKeyEncoding)
{
    return RPCMessage::toMessage(
                QList<RPCField>()
                << RPCField{ownerKey(), iOwner}
                << RPCField{walletIdsKey(), iWalletIds}
                << RPCField{transactionIdKey(), iTransactionId}
                << RPCField{QStringLiteral(kFieldKey_Command), commandValue()},
                iUsername, iPrivateKey, iKeyEncoding
                );
}
