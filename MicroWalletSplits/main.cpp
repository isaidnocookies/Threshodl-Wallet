#include <QDebug>
#include <QVector>
#include <QtMath>
#include <QQueue>
#include <iostream>

#include <microwalletsplitter.h>

bool testValue(double, bool lHideOutput = false);
bool testRangeOfValues(double iStart, double iEnd, bool lHideOutput = false);

int main(int argc, char *argv[])
{
    Q_UNUSED (argc)
    Q_UNUSED (argv)

    double lValue = 1.0;

    testValue(lValue);
    std::cout << "Estimate of Breakdown: " << MicroWalletSplitter::estimateBreakdown(lValue) << std::endl;

    return 0;
}

bool testRangeOfValues(double iStart, double iEnd, bool lHideOutput) {
    double      lStart = iStart;
    double      lEnd = iEnd;
    bool        lFailed = false;

    for (double i = lStart; i <= lEnd; i+= LOWER_LIMIT) {
        if (!testValue(i, true)) {

            if (!lHideOutput) {
                std::cout << std::endl << "FAILED at " << i << std::endl;
            }

            lFailed = true;
        }
    }

    if (lFailed == false && !lHideOutput) {
        std::cout << "PASSED all from " << LOWER_LIMIT << " to " << lEnd << std::endl;
    }

    return !lFailed;
}

bool testValue(double iStartValue, bool lHideOutput) {
    double                  lStartValue = iStartValue;
    std::vector<double>     lBreaks;
    int                     lNumBreaks = MicroWalletSplitter::getMicroWalletValues(lStartValue, lBreaks, 1);
    double                  lTotalFromMicros = 0.0;
    bool                    lComplete;

    int lTempCounter = 0;
    for (auto lValue : lBreaks) {

        if (!lHideOutput)
            std::cout << lValue;

        lTempCounter++;
        if (lTempCounter < static_cast<int>(lBreaks.size())) {

            if (!lHideOutput)
                std::cout << ", ";
        }

        lTotalFromMicros += lValue;
    }

    lComplete = MicroWalletSplitter::checkCompleteness(lBreaks, lStartValue);

    if (!lHideOutput) {
        std::cout << std::endl << std::endl << (lComplete ? "Complete" : "FAIL") << std::endl;

        std::cout << std::endl << "Number of breaks:  " << lNumBreaks << std::endl;
        std::cout << "Total: " << lTotalFromMicros << "   Actual: " << lStartValue << std::endl;
    }

    return lComplete;
}
