#ifndef WALLETGRINDERALPHA_H
#define WALLETGRINDERALPHA_H

#include <QString>

#include <vector>
#include <deque>
#include <cmath>
#include <algorithm>

// Both LOWER_LIMIT and LOWER_LIMIT_POW have to be in sync with each other
//////////////////////////
#define LOWER_LIMIT 0.0001
#define LOWER_LIMIT_POW -4
//////////////////////////

typedef struct BValue {
    int value;
    int multiplier;
} BValue;

class WalletGrinderAlpha
{
public:
    static int getMicroWalletValues (double iInitialAmount, std::vector<double>& oBreaks, int iPasses = 1);
    static int getMicroWalletValues (double iInitialAmount, std::vector<BValue>& oBreaks, int iPasses = 1);
    static bool checkCompleteness (std::vector<double> iBreaks, double iTotal);
    static int estimateBreakdown (double iValue);

    static double getValueFromBValue (BValue iValue);
    static int getPlaceMultiplier (int indexOfDecimal, int currentIndex);
    static BValue getBreakValue (BValue iValue);
    static BValue getBreakValueRemainder(BValue iValue);
    static bool greaterThanBValue (BValue iLHS, BValue iRHS);
    static std::vector<BValue> * getBreaks(double iStartValue, int iPasses = 1);
    static std::vector<BValue> * getBreaks(const QString iStartValue, int iPasses = 1);
};

#endif // WALLETGRINDERALPHA_H
