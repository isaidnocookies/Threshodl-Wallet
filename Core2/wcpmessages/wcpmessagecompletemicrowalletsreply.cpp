#include "wcpmessagecompletemicrowalletsreply.h"

WCPMessageCompleteMicroWalletsReply::WCPMessageCompleteMicroWalletsReply() : WCPMessage()
{ }

WCPMessageCompleteMicroWalletsReply::WCPMessageCompleteMicroWalletsReply(const WCPMessage &iOther) : WCPMessage(iOther)
{ }

WCPMessageCompleteMicroWalletsReply::WCPMessageCompleteMicroWalletsReply(const QString iMessage) : WCPMessage(iMessage)
{ }

QString WCPMessageCompleteMicroWalletsReply::commandValue()
{ return QStringLiteral("completeMicroWallets"); }

QString WCPMessageCompleteMicroWalletsReply::replyCodeKey()
{ return QStringLiteral("replyCode"); }

QString WCPMessageCompleteMicroWalletsReply::transactionIdKey()
{ return QStringLiteral("transactionId"); }

QString WCPMessageCompleteMicroWalletsReply::walletPartialKeysKey()
{ return QStringLiteral("walletPartialKeys"); }

WCPMessageCompleteMicroWalletsReply::ReplyCode WCPMessageCompleteMicroWalletsReply::replyCode() const
{ return static_cast<ReplyCode>(fieldValue(replyCodeKey()).toUInt()); }

QString WCPMessageCompleteMicroWalletsReply::transactionId() const
{ return fieldValue(transactionIdKey()).toString(); }

QMap<QString, QByteArray> WCPMessageCompleteMicroWalletsReply::walletPartialKeys() const
{
    QVariantMap                 lVMap = fieldValue(walletPartialKeysKey()).toMap();
    QMap<QString, QByteArray>   lRet;

    for( QString lKey : lVMap.keys() )
    { lRet[lKey] = QByteArray::fromBase64(lVMap[lKey].toByteArray()); }

    return lRet;
}

QString WCPMessageCompleteMicroWalletsReply::create(const WCPMessageCompleteMicroWalletsReply::ReplyCode iReplyCode, const QMap<QString, QByteArray> iWalletPartialKeys, const QString iTransactionId, const QString iUsername, const QByteArray iPrivateKey, WCPMessage::KeyEncoding iKeyEncoding)
{
    QVariantMap                 lVMap;

    for( QString lKey : iWalletPartialKeys.keys() )
    { lVMap[lKey] = iWalletPartialKeys[lKey].toBase64(); }

    return WCPMessage::toMessage(
                QList<WCPField>()
                << WCPField{replyCodeKey(), static_cast<unsigned int>(iReplyCode)}
                << WCPField{walletPartialKeysKey(), lVMap}
                << WCPField{transactionIdKey(), iTransactionId}
                << WCPField{QStringLiteral(kFieldKey_Command), commandValue()},
                iUsername, iPrivateKey, iKeyEncoding
                );
}
