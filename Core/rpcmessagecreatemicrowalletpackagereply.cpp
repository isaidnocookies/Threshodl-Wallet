#include "rpcmessagecreatemicrowalletpackagereply.h"

RPCMessageCreateMicroWalletPackageReply::RPCMessageCreateMicroWalletPackageReply() : RPCMessage()
{ }

RPCMessageCreateMicroWalletPackageReply::RPCMessageCreateMicroWalletPackageReply(const RPCMessage &iOther)
    : RPCMessage(iOther)
{ }

RPCMessageCreateMicroWalletPackageReply::RPCMessageCreateMicroWalletPackageReply(const QString iMessage)
    : RPCMessage(iMessage)
{ }

QString RPCMessageCreateMicroWalletPackageReply::commandValue()
{ return QStringLiteral("createMicroWalletPackage"); }

QString RPCMessageCreateMicroWalletPackageReply::transactionIdKey()
{ return QStringLiteral("transactionId"); }

QString RPCMessageCreateMicroWalletPackageReply::microWalletsDataKey()
{ return QStringLiteral("microWalletsData"); }

QString RPCMessageCreateMicroWalletPackageReply::transactionId() const
{ return fieldValue(transactionIdKey()).toString(); }

QList<QByteArray> RPCMessageCreateMicroWalletPackageReply::microWalletsData() const
{
    QList<QByteArray>   lRet;
    QVariantList        lList = fieldValue(microWalletsDataKey()).toList();

    for( QVariant lEnt : lList )
    { lRet << QByteArray::fromBase64(lEnt.toByteArray()); }

    return lRet;
}

QString RPCMessageCreateMicroWalletPackageReply::create(const QList<QByteArray> iMicroWalletDatas, const QString iTransactionId, const QString iUsername, const QByteArray iPrivateKey, RPCMessage::KeyEncoding iKeyEncoding)
{
    QVariantList    lMicroWalletDatas;

    for( QByteArray lEnt : iMicroWalletDatas )
    { lMicroWalletDatas << lEnt.toBase64(); }

    return RPCMessage::toMessage(
                QList<RPCField>()
                << RPCField{microWalletsDataKey(), lMicroWalletDatas}
                << RPCField{transactionIdKey(), iTransactionId}
                << RPCField{QStringLiteral(kFieldKey_Command), commandValue()},
                iUsername, iPrivateKey, iKeyEncoding
                );
}
