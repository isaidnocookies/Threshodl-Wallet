#include "bitcoinmicrowallet.h"

#include <QJsonDocument>

BitcoinMicroWallet::~BitcoinMicroWallet()
{ }

BitcoinMicroWallet::BitcoinMicroWallet(const QByteArray iData)
{
    QVariantMap lDataMap;
    lDataMap = QJsonDocument::fromBinaryData(iData).toVariant().toMap();
    _baseClassFromData(lDataMap);
    _microWalletClassFromData(lDataMap);
    _bitcoinWalletClassFromData(lDataMap);
}

QByteArray BitcoinMicroWallet::toData() const
{ return _baseClassToData(_microWalletClassToData(_bitcoinWalletClassToData(QVariantMap()))); }

