#include "grinder.h"
#include "app.h"
#include "modulelinker.h"

#include <QDebug>
#include <cmath>
#include <algorithm>

static GrinderML _gRegisterModuleLinker;

GrinderML::GrinderML()
{ ModuleLinker::registerModule(QStringLiteral("Grinder-1"),GrinderML::creator,GrinderML::doInit,GrinderML::start,GrinderML::startInOwnThread); }

void *GrinderML::creator(void *pointerToAppObject)
{
    Grinder *   lGrinder = new Grinder;
    lGrinder->mApp = reinterpret_cast<App *>(pointerToAppObject);
    return lGrinder;
}

bool GrinderML::doInit(void *pointerToThis, void *pointerToAppObject)
{
    Q_UNUSED(pointerToThis)
    Q_UNUSED(pointerToAppObject)
    return true;
}

bool GrinderML::startInOwnThread()
{ return false; }

bool GrinderML::start(void *pointerToThis, void *pointerToAppObject)
{
    Q_UNUSED(pointerToThis)
    Q_UNUSED(pointerToAppObject)
    return true;
}

Grinder::Grinder()
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

Grinder::~Grinder()
{

}

QStringList Grinder::grindValues(const QString iValue, const QString iCryptoCurrency)
{
    GrindingConstraintsRef  lConstraints    = mGrindingConstraints[iCryptoCurrency];

    if( ! lConstraints )
        return QStringList() << iValue;


    bool            lIsNegative     = false;
    QString         lNValue;

    if( ! _normalizeString(iValue,lNValue,lIsNegative) || lIsNegative )
        return QStringList();

    QStringList     lGrindedResult;
    int             lDoPasses = lConstraints->DefaultPasses;
    QStringMath     lValue{lNValue};
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
            lValueToWorkOn = lNValue;
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

bool Grinder::_normalizeString(const QString iInput, QString &oOutput, bool &oIsNegative)
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

QString Grinder::_divideStringValue(const QString iValue, unsigned int iDivsor)
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

QStringList Grinder::_appendQStringLists(const QStringList iFirst, const QStringList iSecond)
{
    QStringList lResult = iFirst;
    for( auto lE : iSecond )
    { lResult << lE; }
    return lResult;
}

QStringList Grinder::_sortNumberedQStringList(const QStringList iList, bool iHighestFirst)
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

    if( iHighestFirst ) {
        QStringList lOldResult = lResult;
        lResult = QStringList();
        for( QString lE : lOldResult )
        { lResult.prepend(lE); }
    }

    return lResult;
}

QStringList Grinder::_grindValueOnePass(const QString iValue, Grinder::GrindingConstraintsRef iConstraints)
{
    Q_UNUSED(iConstraints)

    double                  lLowerLimit = 0.0001;
    int                     lLowerLimitPow = -4;

    typedef struct BValue {
        int value;
        int multiplier;
    } BValue;

    std::vector<BValue> *   lBreaks = new std::vector<BValue>();
    std::vector<BValue> *   lInitialValues = new std::vector<BValue>();
    QStringList             lStringBreaks;
    QString                 lStartValueString;
    int                     lDecimalIndex;

    auto getBreakValueFunction = [](BValue iInput) -> BValue {
        BValue lBreakValue;
        float lMultiplier = iInput.multiplier;

        if (iInput.value == 1) {
            lBreakValue.multiplier = static_cast<int>(lMultiplier - 1);
            lBreakValue.value = 5;
        } else if (iInput.value == -1) {
            return iInput;
        } else {
            lBreakValue.value = iInput.value / 2;
            lBreakValue.multiplier = iInput.multiplier;
        }

        return lBreakValue;
    };

    auto getBreakValueRemainderFunction = [getBreakValueFunction](BValue iInput) -> BValue {
        BValue lBreakValue = getBreakValueFunction(iInput);
        BValue lBreakValueRemainder;

        if (iInput.value == 1) {
            return lBreakValue;
        }

        lBreakValueRemainder.value = iInput.value - lBreakValue.value;
        lBreakValueRemainder.multiplier = iInput.multiplier;

        return lBreakValueRemainder;
    };

    auto getValueFromBValue = [](BValue iInput) -> double {
        return iInput.value * (std::pow(10.0, iInput.multiplier));
    };

    lStartValueString = iValue;
    lDecimalIndex = lStartValueString.indexOf(".");

    for (int i = 0; i < lStartValueString.count(); i++) {
        QString lSValue = lStartValueString.at(i);
        BValue lNewValue;

        if (lSValue != "." && lSValue != "0") {
            lNewValue.value = lSValue.toInt();

            int powMult = lDecimalIndex - i;
            if (lDecimalIndex > i)
                powMult -= 1;

            lNewValue.multiplier = powMult;

            lInitialValues->push_back(lNewValue);
        }
    }

    while (!lInitialValues->empty()) {
        BValue lCurrentValue = lInitialValues->front();
        lInitialValues->erase(lInitialValues->begin());

        if (lCurrentValue.multiplier == lLowerLimitPow && lCurrentValue.value == 1) {
            lBreaks->push_back(lCurrentValue);
            break;
        }

        if (lInitialValues->size() > 0) {
            while (lCurrentValue.multiplier >= lInitialValues->front().multiplier && lCurrentValue.value >= 1) {
                lBreaks->push_back(getBreakValueFunction(lCurrentValue));
                lCurrentValue = getBreakValueRemainderFunction(lCurrentValue);
                if (lCurrentValue.multiplier == lInitialValues->front().multiplier && lCurrentValue.value == 1) {
                    lBreaks->push_back(lCurrentValue);
                    break;
                }
            }
        } else {
            while (lCurrentValue.multiplier >= lLowerLimitPow && lCurrentValue.value >= 1) {
                lBreaks->push_back(getBreakValueFunction(lCurrentValue));
                lCurrentValue = getBreakValueRemainderFunction(lCurrentValue);

                if (getValueFromBValue(lCurrentValue) <= lLowerLimit) {
                    lBreaks->push_back(lCurrentValue);
                    break;
                }
            }
        }
    }

    for (size_t i = 0; i < lBreaks->size(); i++) {
        lStringBreaks << QString::number(getValueFromBValue(lBreaks->at(static_cast<int>(i))));
    }

    return _sortNumberedQStringList(lStringBreaks);
}
