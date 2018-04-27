#include "walletgrinderv1.h"

#include <QDebug>

bool WalletGrinderV1::_normalizeString(const QString iInput, QString &oOutput, bool &oIsNegative)
{
    oIsNegative = false;
    oOutput     = iInput.trimmed();

    if( oOutput.isEmpty() )
        return false;

    if( oOutput.startsWith(QChar{'-'}) ) {
        oIsNegative = true;
        oOutput = oOutput.mid(1).trimmed();
    }else if( oOutput.startsWith(QChar{'+'}) ) {
        oIsNegative = false;
        oOutput = oOutput.mid(1).trimmed();
    }

    if( oOutput.startsWith(QChar{'.'}) )
        oOutput.prepend(QChar{'0'});

    if( ! oOutput.contains(QChar{'.'}) )
        oOutput.append(QStringLiteral(".0"));

    return true;
}

QString WalletGrinderV1::_divideStringValue(const QString iValue, unsigned int iDivsor)
{
    QString         lValue;
    QString         lResult;
    bool            lIsNegative;
    QString         lValueLeftString;
    QString         lValueRightString;
    unsigned int    lModuloLeft;
    unsigned int    lModuloRight;
    int             lDecPos;
    unsigned int    lWorkingValue;
    bool            lOk;
    unsigned int    lModuloBalance;
    int             lModLimitor;

    try {
        if( ! _normalizeString(iValue,lValue,lIsNegative) )  throw __LINE__;

        lDecPos = lValue.indexOf(QChar{'.'});
        if( lDecPos < 0 ) throw __LINE__;

        lValueLeftString = lValue.left(lDecPos);
        lValueRightString = lValue.mid(lDecPos+1); // Skip '.'

        lWorkingValue = lValueLeftString.toUInt(&lOk,10);
        if( ! lOk ) throw __LINE__;

        lModuloLeft         = (lWorkingValue % iDivsor) * (10 * lValueRightString.length());
        lWorkingValue       = lWorkingValue / iDivsor;
        lValueLeftString    = QString::number(lWorkingValue,10);

        lWorkingValue = lValueRightString.toUInt(&lOk,10);
        if( ! lOk ) throw __LINE__;

        lModuloRight        = (lWorkingValue % iDivsor) + (lModuloLeft % iDivsor);
        lWorkingValue       = (lWorkingValue / iDivsor) + (lModuloLeft / iDivsor);

        lResult             = QStringLiteral("%1.%2").arg(lValueLeftString).arg(lWorkingValue,lValueRightString.length(),10,QChar{'0'});

        for( lModLimitor = 0; lModLimitor < 16 && lModuloRight > 0; lModLimitor++ ) {
            lModuloRight = lModuloRight * 10;
            lModuloBalance = lModuloRight % iDivsor;
            lWorkingValue = lModuloRight / iDivsor;
            lResult = lResult.append(QString::number(lWorkingValue,10));
            lModuloRight = lModuloBalance;
        }

        if( lIsNegative )
            return QStringLiteral("-%1").arg(lResult);
        return lResult;

    } catch(int iLine) {
        qDebug() << "Error line" << iLine;
        return QStringLiteral("0.0");
    }
}

QStringList WalletGrinderV1::_appendQStringLists(const QStringList iFirst, const QStringList iSecond)
{
    QStringList lResult = iFirst;
    for( auto lE : iSecond )
    { lResult << lE; }
    return lResult;
}

QStringList WalletGrinderV1::_sortNumberedQStringList(const QStringList iList)
{
    QStringList lResult;

    for( auto lE : iList )
    {
        if( lResult.isEmpty() ) {
            lResult << lE;
            continue;
        }

        QStringMath lES{lE};
        int lPos = lResult.size() / 2;

        // Up or down?
        if( QStringMath(lResult[lPos]) > lES ) {
            // Down
            for( ; lPos >= 0; lPos-- ) {
                if( QStringMath(lResult[lPos]) < lES ) {
                    lResult.insert(lPos,lE);
                    break;
                }
            }
            if( lPos < 0 )
                lResult.prepend(lE);
        } else {
            // Up
            for( ; lPos < lResult.size(); lPos++ ) {
                if( QStringMath(lResult[lPos]) > lES ) {
                    lResult.insert(lPos,lE);
                    break;
                }
            }

            if( lPos >= lResult.size() )
                lResult.append(lE);
        }
    }

    return lResult;
}

QStringList WalletGrinderV1::_grindValueOnePass(const QString iValue, WalletGrinderV1::GrindingConstraintsRef iConstraints)
{
    Q_UNUSED(iValue) Q_UNUSED(iConstraints)
    return QStringList();
}

WalletGrinderV1::WalletGrinderV1()
{
    GrindingConstraintsRef  lBTC            = GrindingConstraintsRef(new GrindingConstraints_t);
    lBTC->Currency                          = QStringLiteral("BTC");
    lBTC->DefaultPasses                     = 2;
    lBTC->MaxWalletsInt                     = 200;

    for( int lIndex = 1; lIndex <= 10; lIndex++ ) {
        PassesEntryRef  lPassEntry  = PassesEntryRef(new PassesEntry_t);
        lPassEntry->Value           = QStringMath{QStringLiteral("%1.0").arg(lIndex)};
        lPassEntry->NumberOfPasses  = lBTC->DefaultPasses + (lIndex - 1);
        lBTC->CurrencyPasses << lPassEntry;
    }

    mGrindingConstraints[QStringLiteral("BTC")] = lBTC;
}

WalletGrinderV1::~WalletGrinderV1()
{ }

QStringList WalletGrinderV1::grindValue(const QString iValue, const QString iCryptoCurrency)
{
    GrindingConstraintsRef  lConstraints = mGrindingConstraints[iCryptoCurrency];

    if( ! lConstraints )
        return QStringList() << iValue;


    QStringList     lGrindedResult;
    int             lDoPasses = lConstraints->DefaultPasses;
    QStringMath     lValue{iValue};
    QString         lValueToWorkOn;

    // Find out how many passes we should do
    for( auto lEntry : lConstraints->CurrencyPasses )
    {
        if(lValue <= lEntry->Value) {
            lDoPasses = lEntry->NumberOfPasses;
            break;
        }
    }

    // Need to do at least 1 pass
    if( lDoPasses < 1 ) lDoPasses = 1;

    for( int lIndex = 0; lIndex < lDoPasses; lIndex++ ) {
        if( lGrindedResult.isEmpty() )
            lValueToWorkOn = iValue;
        else
            lValueToWorkOn = lGrindedResult.takeFirst();

        lGrindedResult = _sortNumberedQStringList(_appendQStringLists( lGrindedResult, _grindValueOnePass(lValueToWorkOn, lConstraints) ));
    }

    // Is the wallet too large?
    while( lGrindedResult.size() > lConstraints->MaxWalletsInt ) {
        // List is too big, what we do is take the LARGEST value wallets and merge them together until we reach the max number of wallets
        QStringMath lTopValue       = QStringMath{lGrindedResult.takeFirst()};
        QStringMath lNextTopValue   = QStringMath{lGrindedResult.takeFirst()};
        lGrindedResult.prepend((lTopValue.add(lNextTopValue)).toString());
    }

    return lGrindedResult;
}
