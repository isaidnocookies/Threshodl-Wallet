#include "microwallet.h"

#include <QJsonDocument>

QByteArray MicroWallet::_baseClassToData(QVariantMap iMap) const
{
    iMap["value"]           = mValue;
    iMap["longNameType"]    = mLongNameType;
    iMap["shortNameType"]   = mShortNameType;
    iMap["privateKey"]      = mPrivateKey;
    iMap["publicKey"]       = mPublicKey;
    iMap["address"]         = mAddress;
    return QJsonDocument::fromVariant(iMap).toBinaryData();
}

void MicroWallet::_baseClassFromData(QVariantMap iDataMap)
{
    mValue          = iDataMap["value"].toString();
    mLongNameType   = iDataMap["longNameType"].toString();
    mShortNameType  = iDataMap["shortNameType"].toString();
    mPrivateKey     = iDataMap["privateKey"].toByteArray();
    mPublicKey      = iDataMap["publicKey"].toByteArray();
    mAddress        = iDataMap["address"].toByteArray();
}

MicroWallet::MicroWallet(const QByteArray iData)
{
    QVariantMap lDataMap;
    lDataMap = QJsonDocument::fromBinaryData(iData).toVariant().toMap();
    _baseClassFromData(lDataMap);
}

