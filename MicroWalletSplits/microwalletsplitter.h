#ifndef MICROWALLETSPLITTER_H
#define MICROWALLETSPLITTER_H

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

class MicroWalletSplitter
{
public:
    static int getMicroWalletValues (double iInitialAmount, std::vector<double>& oBreaks, int iPasses = 1);
    static int getMicroWalletValues (double iInitialAmount, std::vector<BValue>& oBreaks, int iPasses = 1);
    static bool checkCompleteness (std::vector<double> iBreaks, double iTotal);
    static int estimateBreakdown (double iValue);

private:
    static int getPlaceMultiplier (int indexOfDecimal, int currentIndex);
    static double getValueFromBValue (BValue iValue);
    static BValue getBreakValue (BValue iValue);
    static BValue getBreakValueRemainder(BValue iValue);
    static std::vector<BValue> * getBreaks(double iStartValue, int iPasses = 1);
    static bool greaterThanBValue (BValue iLHS, BValue iRHS);
};

#endif // MICROWALLETSPLITTER_H
