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

QString RPCMessageCreateMicroWalletPackageRequest::txIdKey()
{ return QStringLiteral("txId"); }

QString RPCMessageCreateMicroWalletPackageRequest::voutKey()
{ return QStringLiteral("vout"); }

QString RPCMessageCreateMicroWalletPackageRequest::outputBalanceAddressKey()
{ return QStringLiteral("outputBalanceAddress"); }

QString RPCMessageCreateMicroWalletPackageRequest::outputBalanceKey()
{ return QStringLiteral("outputBalance"); }

QString RPCMessageCreateMicroWalletPackageRequest::txId() const
{ return fieldValue(txIdKey()).toString(); }

QString RPCMessageCreateMicroWalletPackageRequest::vout() const
{ return fieldValue(voutKey()).toString(); }

QString RPCMessageCreateMicroWalletPackageRequest::outputBalanceAddress() const
{ return fieldValue(outputBalanceAddressKey()).toString(); }

QString RPCMessageCreateMicroWalletPackageRequest::outputBalance() const
{ return fieldValue(outputBalanceKey()).toString(); }

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

QString RPCMessageCreateMicroWalletPackageRequest::createBtc(const QString iCryptoValue, const QString iTxId, const QString iVout, const QString iTransactionId, const QString iUsername, const QByteArray iPrivateKey, const QString iOutputBalanceAddress, const QString iOutputBalance, RPCMessage::KeyEncoding iKeyEncoding)
{
    return RPCMessage::toMessage(
                QList<RPCField>()
                << RPCField{cryptoTypeShortNameKey(), QStringLiteral("btc")}
                << RPCField{cryptoValueKey(), iCryptoValue}
                << RPCField{outputBalanceAddressKey(), iOutputBalanceAddress}
                << RPCField{outputBalanceKey(), iOutputBalance}
                << RPCField{txIdKey(), iTxId}
                << RPCField{voutKey(), iVout}
                << RPCField{transactionIdKey(), iTransactionId}
                << RPCField{QStringLiteral(kFieldKey_Command), commandValue()},
                iUsername, iPrivateKey, iKeyEncoding
                );
}

