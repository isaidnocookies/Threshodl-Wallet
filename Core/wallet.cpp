#include "wallet.h"

#include <QJsonDocument>

QByteArray Wallet::_baseClassToData(QVariantMap iMap) const
{
    iMap["value"]           = mValue;
    iMap["longNameType"]    = mLongNameType;
    iMap["shortNameType"]   = mShortNameType;
    iMap["privateKey"]      = mPrivateKey.toBase64();
    iMap["publicKey"]       = mPublicKey.toBase64();
    iMap["address"]         = mAddress.toBase64();
    return QJsonDocument::fromVariant(iMap).toBinaryData();
}

void Wallet::_baseClassFromData(QVariantMap iDataMap)
{
    mValue          = iDataMap["value"].toString();
    mLongNameType   = iDataMap["longNameType"].toString();
    mShortNameType  = iDataMap["shortNameType"].toString();
    mPrivateKey     = QByteArray::fromBase64(iDataMap["privateKey"].toByteArray());
    mPublicKey      = QByteArray::fromBase64(iDataMap["publicKey"].toByteArray());
    mAddress        = QByteArray::fromBase64(iDataMap["address"].toByteArray());
}

Wallet::Wallet(const QByteArray iData)
{
    QVariantMap lDataMap;
    lDataMap = QJsonDocument::fromBinaryData(iData).toVariant().toMap();
    _baseClassFromData(lDataMap);
}

