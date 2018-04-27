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

QString RPCMessageCreateMicroWalletPackageRequest::transactionIdKey()
{ return QStringLiteral("transactionId"); }

QString RPCMessageCreateMicroWalletPackageRequest::cryptoTypeShortName() const
{ return fieldValue(cryptoTypeShortNameKey()).toString(); }

QString RPCMessageCreateMicroWalletPackageRequest::cryptoValue() const
{ return fieldValue(cryptoValueKey()).toString(); }

QString RPCMessageCreateMicroWalletPackageRequest::transactionId() const
{ return fieldValue(transactionIdKey()).toString(); }

QString RPCMessageCreateMicroWalletPackageRequest::inputCountKey()
{ return QStringLiteral("inputCount"); }

QString RPCMessageCreateMicroWalletPackageRequest::outputCountKey()
{ return QStringLiteral("outputCount"); }

QString RPCMessageCreateMicroWalletPackageRequest::chainTypeKey()
{ return QStringLiteral("chainType"); }

int RPCMessageCreateMicroWalletPackageRequest::inputCount() const
{ return fieldValue(inputCountKey()).toInt(); }

int RPCMessageCreateMicroWalletPackageRequest::outputCount() const
{ return fieldValue(outputCountKey()).toInt(); }

BitcoinWallet::ChainType RPCMessageCreateMicroWalletPackageRequest::chainType() const
{ return static_cast<BitcoinWallet::ChainType>(fieldValue(chainTypeKey()).toUInt()); }

QString RPCMessageCreateMicroWalletPackageRequest::create(const QString iCryptoTypeShortName, const QString iCryptoValue, const QString iTransactionId, const QString iUsername, const QByteArray iPrivateKey, RPCMessage::KeyEncoding iKeyEncoding)
{
    return RPCMessage::toMessage(
                QList<RPCField>()
                << RPCField{cryptoTypeShortNameKey(), iCryptoTypeShortName}
                << RPCField{cryptoValueKey(), iCryptoValue}
                << RPCField{transactionIdKey(), iTransactionId}
                << RPCField{QStringLiteral(kFieldKey_Command), commandValue()},
                iUsername, iPrivateKey, iKeyEncoding
                );
}

QString RPCMessageCreateMicroWalletPackageRequest::createBtc(const QString iCryptoValue, int iInputCount, int iOutputCount, const BitcoinWallet::ChainType iChainType, const QString iTransactionId, const QString iUsername, const QByteArray iPrivateKey, RPCMessage::KeyEncoding iKeyEncoding)
{
    return RPCMessage::toMessage(
                QList<RPCField>()
                << RPCField{cryptoTypeShortNameKey(), QStringLiteral("btc")}
                << RPCField{cryptoValueKey(), iCryptoValue}
                << RPCField{inputCountKey(), iInputCount}
                << RPCField{outputCountKey(), iOutputCount}
                << RPCField{chainTypeKey(), static_cast<unsigned int>(iChainType)}
                << RPCField{transactionIdKey(), iTransactionId}
                << RPCField{QStringLiteral(kFieldKey_Command), commandValue()},
                iUsername, iPrivateKey, iKeyEncoding
                );
}

