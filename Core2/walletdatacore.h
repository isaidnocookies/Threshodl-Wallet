#ifndef WALLETDATACORE_H
#define WALLETDATACORE_H

#include <QByteArray>
#include <QString>
#include <QVariantMap>
#include <QJsonDocument>

using WalletDataCore = QVariantMap;

#define WalletDataCoreValueToString(c,k)            (c[k].toString())
#define WalletDataCoreValueToByteArray(c,k)         (QByteArray::fromBase64(c[k].toByteArray()))
#define WalletDataCoreValueToUInt(c,k)              (c[k].toUInt())
#define WalletDataCoreValueToInt(c,k)               (c[k].toInt())
#define WalletDataCoreValueFromString(c,k,v)        c[k] = v
#define WalletDataCoreValueFromByteArray(c,k,v)     c[k] = v.toBase64()
#define WalletDataCoreValueFromUInt(c,k,v)          c[k] = static_cast<unsigned int>(v)
#define WalletDataCoreValueFromInt(c,k,v)           c[k] = static_cast<int>(v)

#define WalletDataCoreSerialize(c)                  (QJsonDocument::fromVariant(c).toJson(QJsonDocument::Compact))

bool WalletDataCoreDeserialize(const QByteArray iData, WalletDataCore &oWalletDataCore);

#endif // WALLETDATACORE_H
