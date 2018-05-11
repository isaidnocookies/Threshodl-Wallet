#include "wcpmessagecreatemicrowalletpackagerequest.h"

WCPMessageCreateMicroWalletPackageRequest::WCPMessageCreateMicroWalletPackageRequest() : WCPMessage()
{ }

WCPMessageCreateMicroWalletPackageRequest::WCPMessageCreateMicroWalletPackageRequest(const WCPMessage &iOther)
    : WCPMessage(iOther)
{ }

WCPMessageCreateMicroWalletPackageRequest::WCPMessageCreateMicroWalletPackageRequest(const QString iMessage)
    : WCPMessage(iMessage)
{ }

QString WCPMessageCreateMicroWalletPackageRequest::commandValue()
{ return QStringLiteral("createMicroWalletPackage"); }

QString WCPMessageCreateMicroWalletPackageRequest::cryptoTypeShortNameKey()
{ return QStringLiteral("cryptoTypeShortName"); }

QString WCPMessageCreateMicroWalletPackageRequest::cryptoValueKey()
{ return QStringLiteral("cryptoValue"); }

QString WCPMessageCreateMicroWalletPackageRequest::transactionIdKey()
{ return QStringLiteral("transactionId"); }

QString WCPMessageCreateMicroWalletPackageRequest::cryptoTypeShortName() const
{ return fieldValue(cryptoTypeShortNameKey()).toString(); }

QString WCPMessageCreateMicroWalletPackageRequest::cryptoValue() const
{ return fieldValue(cryptoValueKey()).toString(); }

QString WCPMessageCreateMicroWalletPackageRequest::transactionId() const
{ return fieldValue(transactionIdKey()).toString(); }

QString WCPMessageCreateMicroWalletPackageRequest::inputCountKey()
{ return QStringLiteral("inputCount"); }

QString WCPMessageCreateMicroWalletPackageRequest::outputCountKey()
{ return QStringLiteral("outputCount"); }

QString WCPMessageCreateMicroWalletPackageRequest::chainTypeKey()
{ return QStringLiteral("chainType"); }

int WCPMessageCreateMicroWalletPackageRequest::inputCount() const
{ return fieldValue(inputCountKey()).toInt(); }

int WCPMessageCreateMicroWalletPackageRequest::outputCount() const
{ return fieldValue(outputCountKey()).toInt(); }

BitcoinWallet::ChainType WCPMessageCreateMicroWalletPackageRequest::chainType() const
{ return static_cast<BitcoinWallet::ChainType>(fieldValue(chainTypeKey()).toUInt()); }

QString WCPMessageCreateMicroWalletPackageRequest::create(const QString iCryptoTypeShortName, const QString iCryptoValue, const QString iTransactionId, const QString iUsername, const QByteArray iPrivateKey, WCPMessage::KeyEncoding iKeyEncoding)
{
    return WCPMessage::toMessage(
                QList<WCPField>()
                << WCPField{cryptoTypeShortNameKey(), iCryptoTypeShortName}
                << WCPField{cryptoValueKey(), iCryptoValue}
                << WCPField{transactionIdKey(), iTransactionId}
                << WCPField{QStringLiteral(kFieldKey_Command), commandValue()},
                iUsername, iPrivateKey, iKeyEncoding
                );
}

QString WCPMessageCreateMicroWalletPackageRequest::createBtc(const QString iCryptoValue, int iInputCount, int iOutputCount, const BitcoinWallet::ChainType iChainType, const QString iTransactionId, const QString iUsername, const QByteArray iPrivateKey, WCPMessage::KeyEncoding iKeyEncoding)
{
    return WCPMessage::toMessage(
                QList<WCPField>()
                << WCPField{cryptoTypeShortNameKey(), QStringLiteral("btc")}
                << WCPField{cryptoValueKey(), iCryptoValue}
                << WCPField{inputCountKey(), iInputCount}
                << WCPField{outputCountKey(), iOutputCount}
                << WCPField{chainTypeKey(), static_cast<unsigned int>(iChainType)}
                << WCPField{transactionIdKey(), iTransactionId}
                << WCPField{QStringLiteral(kFieldKey_Command), commandValue()},
                iUsername, iPrivateKey, iKeyEncoding
                );
}

