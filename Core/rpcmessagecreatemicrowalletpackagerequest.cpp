#include "rpcmessagecreatemicrowalletpackagerequest.h"

RPCMessageCreateMicroWalletPackageRequest::RPCMessageCreateMicroWalletPackageRequest() : RPCMessage()
{ }

RPCMessageCreateMicroWalletPackageRequest::RPCMessageCreateMicroWalletPackageRequest(const RPCMessage &iOther)
    : RPCMessage(iOther)
{ }

RPCMessageCreateMicroWalletPackageRequest::RPCMessageCreateMicroWalletPackageRequest(const QString iMessage)
    : RPCMessage(iMessage)
{ }

QString RPCMessageCreateMicroWalletPackageRequest::commandValue()
{ return QStringLiteral("createMicroWalletPackage"); }

QString RPCMessageCreateMicroWalletPackageRequest::cryptoTypeShortNameKey()
{ return QStringLiteral("cryptoTypeShortName"); }

QString RPCMessageCreateMicroWalletPackageRequest::cryptoValueKey()
{ return QStringLiteral("cryptoValue"); }

QString RPCMessageCreateMicroWalletPackageRequest::cryptoTypeShortName() const
{ return fieldValue(cryptoTypeShortNameKey()).toString(); }

QString RPCMessageCreateMicroWalletPackageRequest::cryptoValue() const
{ return fieldValue(cryptoValueKey()).toString(); }

QString RPCMessageCreateMicroWalletPackageRequest::create(const QString iCryptoTypeShortName, const QString iCryptoValue, const QString iUsername, const QByteArray iPrivateKey, RPCMessage::KeyEncoding iKeyEncoding)
{
    return RPCMessage::toMessage(
                QList<RPCField>()
                << RPCField{cryptoTypeShortNameKey(), iCryptoTypeShortName}
                << RPCField{cryptoValueKey(), iCryptoValue}
                << RPCField{QStringLiteral(kFieldKey_Command), commandValue()},
                iUsername, iPrivateKey, iKeyEncoding
                );
}

