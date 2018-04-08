#include "microwallet.h"

#include <QJsonDocument>

QVariantMap MicroWallet::_microWalletClassToData(QVariantMap iMap) const
{
    iMap["privateKeyComplete"]  = mPrivateKeyComplete;
    iMap["microWalletId"]       = mMicroWalletId.toBase64();
    return iMap;
}

void MicroWallet::_microWalletClassFromData(QVariantMap iDataMap)
{
    mPrivateKeyComplete = iDataMap["privateKeyComplete"].toBool();
    mMicroWalletId      = QByteArray::fromBase64(iDataMap["microWalletId"].toByteArray());
}

MicroWallet::MicroWallet(const QByteArray iData)
{
    QVariantMap lDataMap;
    lDataMap = QJsonDocument::fromBinaryData(iData).toVariant().toMap();
    _baseClassFromData(lDataMap);
    _microWalletClassFromData(lDataMap);
}

void MicroWallet::setCompleteKeyComplete(bool iComplete)
{ mPrivateKeyComplete = iComplete; }

bool MicroWallet::completePrivateKey(const QByteArray iNewPrivateKey)
{
    if( mPrivateKeyComplete ) return false;
    mPrivateKeyComplete = true;
    mPrivateKey = iNewPrivateKey;
    return true;
}

void MicroWallet::setMicroWalletId(const QByteArray iMicroWalletId)
{ mMicroWalletId = iMicroWalletId; }

QByteArray MicroWallet::microWalletId() const
{ return mMicroWalletId; }

QByteArray MicroWallet::toData() const
{ return _baseClassToData(_microWalletClassToData(QVariantMap())); }

