#include <QCoreApplication>
#include <QDebug>

#include "qstringmath.h"

void test1();
void test2();
void test3();

int main(int argc, char *argv[])
{
    Q_UNUSED (argc)
    Q_UNUSED (argv)

    test1();
    test2();
    test3();
}

void test1() {
    qDebug() << "\n\n" << "Test1 - Add Strings\n";
    QString lValue1 = "987659.98764567";
    QString lValue2 = "0.098765439801";

    QStringMath lV1 (lValue1);

    qDebug() << lValue1 << " + " << lValue2 << "  =  " << (lV1 + lValue2).toString();
}

void test2() {
    qDebug() << "\n\n" << "Test2 - Standardize Strings\n";
    QString lValue1 = "0.111000";
    QString lValue2 = "1000.987";

    QString lValue1s;
    QString lValue2s;

    QStringMath::standardizeStrings(lValue1, lValue2, lValue1s, lValue2s);

    qDebug() << lValue1s << "       " << lValue2s;
}

void test3() {
    qDebug() << "\n\n" << "Test3 - Subtract Strings\n";
    QString lValue1 = "10.17";
    QString lValue2 = "1.09";

    QStringMath lV1 (lValue1);

    qDebug() << lValue1 << " - " << lValue2 << "  =  " << (lV1 - lValue2).toString();
}
