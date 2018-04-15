#include "rpcmessagecompletemicrowalletsrequest.h"

RPCMessageCompleteMicroWalletsRequest::RPCMessageCompleteMicroWalletsRequest() : RPCMessage()
{ }

RPCMessageCompleteMicroWalletsRequest::RPCMessageCompleteMicroWalletsRequest(const RPCMessage &iOther) : RPCMessage(iOther)
{ }

RPCMessageCompleteMicroWalletsRequest::RPCMessageCompleteMicroWalletsRequest(const QString iMessage) : RPCMessage(iMessage)
{ }

QString RPCMessageCompleteMicroWalletsRequest::commandValue()
{ return QStringLiteral("completeMicroWallets"); }

QString RPCMessageCompleteMicroWalletsRequest::transactionIdKey()
{ return QStringLiteral("transactionId"); }

QString RPCMessageCompleteMicroWalletsRequest::walletIdsKey()
{ return QStringLiteral("walletIds"); }

QString RPCMessageCompleteMicroWalletsRequest::transactionId() const
{ return fieldValue(transactionIdKey()).toString(); }

QStringList RPCMessageCompleteMicroWalletsRequest::walletIds() const
{ return fieldValue(walletIdsKey()).toStringList(); }

QString RPCMessageCompleteMicroWalletsRequest::create(const QStringList iWalletIds, const QString iTransactionId, const QString iUsername, const QByteArray iPrivateKey, RPCMessage::KeyEncoding iKeyEncoding)
{
    return RPCMessage::toMessage(
                QList<RPCField>()
                << RPCField{walletIdsKey(), iWalletIds}
                << RPCField{transactionIdKey(), iTransactionId}
                << RPCField{QStringLiteral(kFieldKey_Command), commandValue()},
                iUsername, iPrivateKey, iKeyEncoding
                );
}
