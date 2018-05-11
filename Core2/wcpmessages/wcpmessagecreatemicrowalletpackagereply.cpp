#include "wcpmessagecreatemicrowalletpackagereply.h"

WCPMessageCreateMicroWalletPackageReply::WCPMessageCreateMicroWalletPackageReply() : WCPMessage()
{ }

WCPMessageCreateMicroWalletPackageReply::WCPMessageCreateMicroWalletPackageReply(const WCPMessage &iOther)
    : WCPMessage(iOther)
{ }

WCPMessageCreateMicroWalletPackageReply::WCPMessageCreateMicroWalletPackageReply(const QString iMessage)
    : WCPMessage(iMessage)
{ }

QString WCPMessageCreateMicroWalletPackageReply::commandValue()
{ return QStringLiteral("createMicroWalletPackage"); }

QString WCPMessageCreateMicroWalletPackageReply::transactionIdKey()
{ return QStringLiteral("transactionId"); }

QString WCPMessageCreateMicroWalletPackageReply::replyCodeKey()
{ return QStringLiteral("replyCode"); }

QString WCPMessageCreateMicroWalletPackageReply::microWalletsDataKey()
{ return QStringLiteral("microWalletsData"); }

WCPMessageCreateMicroWalletPackageReply::ReplyCode WCPMessageCreateMicroWalletPackageReply::replyCode() const
{ return static_cast<ReplyCode>(fieldValue(replyCodeKey()).toUInt()); }

QString WCPMessageCreateMicroWalletPackageReply::transactionId() const
{ return fieldValue(transactionIdKey()).toString(); }

QList<QByteArray> WCPMessageCreateMicroWalletPackageReply::microWalletsData() const
{
    QList<QByteArray>   lRet;
    QVariantList        lList = fieldValue(microWalletsDataKey()).toList();

    for( QVariant lEnt : lList )
    { lRet << QByteArray::fromBase64(lEnt.toByteArray()); }

    return lRet;
}

QString WCPMessageCreateMicroWalletPackageReply::estimatedFeesKey()
{ return QStringLiteral("estimatedFees"); }

QString WCPMessageCreateMicroWalletPackageReply::estimatedFees() const
{ return fieldValue(estimatedFeesKey()).toString(); }

QString WCPMessageCreateMicroWalletPackageReply::create(const ReplyCode iReplyCode, const QList<QByteArray> iMicroWalletDatas, const QString iTransactionId, const QString iUsername, const QByteArray iPrivateKey, WCPMessage::KeyEncoding iKeyEncoding)
{
    QVariantList    lMicroWalletDatas;

    for( QByteArray lEnt : iMicroWalletDatas )
    { lMicroWalletDatas << lEnt.toBase64(); }

    return WCPMessage::toMessage(
                QList<WCPField>()
                << WCPField{replyCodeKey(), static_cast<unsigned int>(iReplyCode)}
                << WCPField{microWalletsDataKey(), lMicroWalletDatas}
                << WCPField{transactionIdKey(), iTransactionId}
                << WCPField{QStringLiteral(kFieldKey_Command), commandValue()},
                iUsername, iPrivateKey, iKeyEncoding
                );
}

QString WCPMessageCreateMicroWalletPackageReply::createBtc(const WCPMessageCreateMicroWalletPackageReply::ReplyCode iReplyCode, const QList<QByteArray> iMicroWalletDatas, const QString iTransactionId, const QString iEstimatedFees, const QString iUsername, const QByteArray iPrivateKey, WCPMessage::KeyEncoding iKeyEncoding)
{
    QVariantList    lMicroWalletDatas;

    for( QByteArray lEnt : iMicroWalletDatas )
    { lMicroWalletDatas << lEnt.toBase64(); }

    return WCPMessage::toMessage(
                QList<WCPField>()
                << WCPField{replyCodeKey(), static_cast<unsigned int>(iReplyCode)}
                << WCPField{microWalletsDataKey(), lMicroWalletDatas}
                << WCPField{transactionIdKey(), iTransactionId}
                << WCPField{estimatedFeesKey(), iEstimatedFees}
                << WCPField{QStringLiteral(kFieldKey_Command), commandValue()},
                iUsername, iPrivateKey, iKeyEncoding
                );
}
