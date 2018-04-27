#ifndef WALLETGRINDERV1_H
#define WALLETGRINDERV1_H

#include "walletgrinder.h"

#include <QSharedPointer>

class WalletGrinderV1 : public WalletGrinder
{
protected:
    typedef struct {
        QStringMath             Value;
        int                     NumberOfPasses;
    } PassesEntry_t;

    typedef QSharedPointer<PassesEntry_t>           PassesEntryRef;

    typedef struct {
        QString                 Currency;
        int                     DefaultPasses;
        int                     MaxWalletsInt;
        QList<PassesEntryRef>   CurrencyPasses;
    } GrindingConstraints_t;

    typedef QSharedPointer<GrindingConstraints_t>   GrindingConstraintsRef;

    QMap< QString, GrindingConstraintsRef >         mGrindingConstraints;

    bool _normalizeString(const QString iInput, QString &oOutput, bool &oIsNegative);
    QString _divideStringValue(const QString iValue, unsigned int iDivsor);
    QStringList _appendQStringLists(const QStringList iFirst, const QStringList iSecond);
    QStringList _sortNumberedQStringList(const QStringList iList);
    QStringList _grindValueOnePass(const QString iValue, GrindingConstraintsRef iConstraints);

public:
    WalletGrinderV1();
    virtual ~WalletGrinderV1();

    QStringList grindValue(const QString iValue, const QString iCryptoCurrency = QStringLiteral("BTC")) override;
};

#endif // WALLETGRINDERV1_H
