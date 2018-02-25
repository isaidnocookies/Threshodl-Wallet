#ifndef ENCAPSULATEDMICROWALLET_H
#define ENCAPSULATEDMICROWALLET_H

#include "microwallet.h"

#include <QSharedPointer>

typedef QSharedPointer<MicroWallet> (*MicroWalletConstructorFromData)(const QByteArray iData);

class EncapsulatedMicroWallet
{
private:
    QSharedPointer<MicroWallet>     mMicroWallet;

    EncapsulatedMicroWallet();
public:
    static void registerMicroWalletConstructor(const QString iShortNameType, MicroWalletConstructorFromData iConstructor);

    QByteArray                                      toData() const;
    static QSharedPointer<EncapsulatedMicroWallet>  fromData(const QByteArray iData);

    QSharedPointer<MicroWallet> microWallet() const;
    void                        setMicroWallet(QSharedPointer<MicroWallet> iMicroWallet);
};

#endif // ENCAPSULATEDMICROWALLET_H
