#include "rpcmessagecompletemicrowalletsreply.h"

RPCMessageCompleteMicroWalletsReply::RPCMessageCompleteMicroWalletsReply() : RPCMessage()
{ }

RPCMessageCompleteMicroWalletsReply::RPCMessageCompleteMicroWalletsReply(const RPCMessage &iOther) : RPCMessage(iOther)
{ }

RPCMessageCompleteMicroWalletsReply::RPCMessageCompleteMicroWalletsReply(const QString iMessage) : RPCMessage(iMessage)
{ }

QString RPCMessageCompleteMicroWalletsReply::commandValue()
{ return QStringLiteral("completeMicroWallets"); }

QString RPCMessageCompleteMicroWalletsReply::replyCodeKey()
{ return QStringLiteral("replyCode"); }

QString RPCMessageCompleteMicroWalletsReply::transactionIdKey()
{ return QStringLiteral("transactionId"); }

QString RPCMessageCompleteMicroWalletsReply::walletPartialKeysKey()
{ }

RPCMessageCompleteMicroWalletsReply::ReplyCode RPCMessageCompleteMicroWalletsReply::replyCode() const
{ return static_cast<ReplyCode>(fieldValue(replyCodeKey()).toUInt()); }

QString RPCMessageCompleteMicroWalletsReply::transactionId() const
{ return fieldValue(transactionIdKey()).toString(); }

QMap<QString, QByteArray> RPCMessageCompleteMicroWalletsReply::walletPartialKeys() const
{
    QVariantMap                 lVMap = fieldValue(transactionIdKey()).toMap();
    QMap<QString, QByteArray>   lRet;

    for( QString lKey : lVMap.keys() )
    { lRet[lKey] = QByteArray::fromBase64(lVMap[lKey].toByteArray()); }

    return lRet;
}

QString RPCMessageCompleteMicroWalletsReply::create(const RPCMessageCompleteMicroWalletsReply::ReplyCode iReplyCode, const QMap<QString, QByteArray> iWalletPartialKeys, const QString iTransactionId, const QString iUsername, const QByteArray iPrivateKey, RPCMessage::KeyEncoding iKeyEncoding)
{
    QVariantMap                 lVMap;

    for( QString lKey : iWalletPartialKeys.keys() )
    { lVMap[lKey] = iWalletPartialKeys[lKey].toBase64(); }

    return RPCMessage::toMessage(
                QList<RPCField>()
                << RPCField{replyCodeKey(), static_cast<unsigned int>(iReplyCode)}
                << RPCField{walletPartialKeysKey(), lVMap}
                << RPCField{transactionIdKey(), iTransactionId}
                << RPCField{QStringLiteral(kFieldKey_Command), commandValue()},
                iUsername, iPrivateKey, iKeyEncoding
                );
}
