#include "encapsulatedmicrowallet.h"

#include <QMap>
#include <QMutex>
#include <QJsonDocument>

static QMutex                                           _sConstructorsLock;
static QMap< QString, MicroWalletConstructorFromData >  _sConstructors;

EncapsulatedMicroWallet::EncapsulatedMicroWallet()
{

}

void EncapsulatedMicroWallet::registerMicroWalletConstructor(const QString iShortNameType, MicroWalletConstructorFromData iConstructor)
{
    QMutexLocker l{&_sConstructorsLock};
    _sConstructors[iShortNameType] = iConstructor;
}

QByteArray EncapsulatedMicroWallet::toData() const
{
    QVariantMap lMap;

    if(mMicroWallet) {
        lMap["microWalletType"] = mMicroWallet->shortNameType();
        lMap["microWallet"]     = mMicroWallet->toData();
    }

    return QJsonDocument::fromVariant(lMap).toBinaryData();
}

QSharedPointer<EncapsulatedMicroWallet> EncapsulatedMicroWallet::fromData(const QByteArray iData)
{
    QSharedPointer<EncapsulatedMicroWallet>     lResult;
    QVariantMap                                 lMap            = QJsonDocument::fromBinaryData(iData).toVariant().toMap();
    QString                                     lType           = lMap["microWalletType"].toString();
    MicroWalletConstructorFromData              lConstructor    = nullptr;

    if( ! lType.isEmpty() ) {
        _sConstructorsLock.lock();
        if( _sConstructors.contains(lType) ) {
            lConstructor = _sConstructors[lType];
        }
        _sConstructorsLock.unlock();
    }

    if( lConstructor ) {
        auto lMicroWallet = lConstructor(lMap["microWallet"].toByteArray());
        lResult = QSharedPointer<EncapsulatedMicroWallet>(new EncapsulatedMicroWallet);
        lResult->setMicroWallet(lMicroWallet);
    }

    return lResult;
}

QSharedPointer<MicroWallet> EncapsulatedMicroWallet::microWallet() const
{
    return mMicroWallet;
}

void EncapsulatedMicroWallet::setMicroWallet(QSharedPointer<MicroWallet> iMicroWallet)
{
    mMicroWallet = iMicroWallet;
}
