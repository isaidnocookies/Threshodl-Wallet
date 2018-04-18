#include "qstringmath.h"

#include <QRegExp>
#include <QStack>
#include <math.h>
#include <QDebug>

QStringMath::QStringMath() { mValue = "0.0"; }
QStringMath::QStringMath(const QString iString) { mValue = iString; }
QStringMath::QStringMath(const QStringMath &other) { this->mValue = other.mValue; }

QStringMath QStringMath::add(const QString other)
{
    return (*this + other);
}

QStringMath QStringMath::add(const QStringMath &other)
{
    return (*this + other);
}

QStringMath QStringMath::subtract(const QString other)
{
    return (*this - other);
}

QStringMath QStringMath::subtract(const QStringMath &other)
{
    return (*this - other);
}

QStringMath QStringMath::operator +(const QString other) { return (*this + QStringMath(other)); }
QStringMath QStringMath::operator +(const QStringMath &other)
{
    // ONLY WORKS FOR POSITIVE NUMBERS FOR NOW
    QString     lValue1s = mValue;
    QString     lValue2s = other.mValue;
    int         lIndex;

    standardizeStrings(mValue, other.mValue, lValue1s, lValue2s);
    lIndex = lValue1s.indexOf(".");

    lValue1s.remove(lIndex, 1);
    lValue2s.remove(lIndex, 1);

    QString lNewValue = "";

    int lFromLast = 0;
    for (int i = lValue1s.size() - 1; i >= 0; i--) {
        int lResult = QString(lValue1s.at(i)).toInt() + QString(lValue2s.at(i)).toInt() + lFromLast;
        int lCarry = lResult / 10;
        int lValue = lResult - (lCarry * 10);

        lNewValue.prepend(QString::number(lValue));
        lFromLast = lCarry;
    }

    lNewValue.insert(lIndex, ".");
    return QStringMath(lNewValue).toString();
}

QStringMath QStringMath::operator -(const QString other) { return (*this - QStringMath(other)); }
QStringMath QStringMath::operator -(const QStringMath &other)
{
    //sanity check
    if (mValue.toDouble() - other.mValue.toDouble() < 0) {
        return QStringMath("0");
    }

    QString lValue1s;
    QString lValue2s;
    QStack<QString> lVStack1;
    QStack<QString> lVStack2;
    QStack<QString> lTempStack;
    QString         lOutput = "";

    standardizeStrings(mValue, other.mValue, lValue1s, lValue2s);

    for (int i = 0; i < lValue1s.size(); i++) {
        lVStack1.push(lValue1s.at(i));
        lVStack2.push(lValue2s.at(i));
    }

    while (!lVStack1.isEmpty() && !lVStack2.isEmpty()) {
        QStringMath lTemp1 (lVStack1.pop());
        QStringMath lTemp2 (lVStack2.pop());

        if (lTemp1.mValue == ".") {
            lOutput.prepend(".");
        } else if (lTemp1 >= lTemp2) {
            lOutput.prepend(QString::number(lTemp1.toInt() - lTemp2.toInt()));
        } else if (lTemp1 < lTemp2) {
            while (!lVStack1.isEmpty() && (lVStack1.top() == "." || lVStack1.top() == "0")) {
                lTempStack.push(lVStack1.pop());
            }

            lVStack1.push(QString::number(lVStack1.pop().toInt() - 1));

            while (!lTempStack.isEmpty()) {
                if (lTempStack.top() == ".") {
                    lVStack1.push(lTempStack.pop());
                } else if (lTempStack.top() == "0") {
                    lVStack1.push("9");
                    lTempStack.pop();
                } else {
                    qDebug() << "Shouldn't happen...";
                }
            }

            lTemp1 = lTemp1 + "10";

            lOutput.prepend(QString::number(lTemp1.toInt() - lTemp2.toInt()));
        }
    }

    return lOutput;
}

bool QStringMath::operator <(const QString other) { return (*this < QStringMath(other)); }
bool QStringMath::operator <(const QStringMath &other)
{
    QString lValue1s = mValue;
    QString lValue2s = other.mValue;

    standardizeStrings(mValue, other.mValue, lValue1s, lValue2s);

    for (int i = 0; i < lValue1s.size() - 1; i++) {
        if (lValue1s.at(i) == lValue2s.at(i)) {
            continue;
        }

        if (lValue1s.at(i) > lValue2s.at(i)) {
            return false;
        }

        if (lValue1s.at(i) < lValue2s.at(i)) {
            return true;
        }
    }

    return false;
}

bool QStringMath::operator >(const QString other) { return (*this > QStringMath(other)); }
bool QStringMath::operator >(const QStringMath &other)
{
    QString lValue1s = mValue;
    QString lValue2s = other.mValue;

    standardizeStrings(mValue, other.mValue, lValue1s, lValue2s);

    for (int i = 0; i < lValue1s.size() - 1; i++) {
        if (lValue1s.at(i) == lValue2s.at(i)) {
            continue;
        }

        if (lValue1s.at(i) < lValue2s.at(i)) {
            return false;
        }

        if (lValue1s.at(i) > lValue2s.at(i)) {
            return true;
        }
    }

    return false;
}

bool QStringMath::operator >=(const QString other) { return (*this >= QStringMath(other)); }
bool QStringMath::operator >=(const QStringMath &other)
{
    QString lValue1s = mValue;
    QString lValue2s = other.mValue;

    standardizeStrings(mValue, other.mValue, lValue1s, lValue2s);

    for (int i = 0; i < lValue1s.size() - 1; i++) {
        if (lValue1s.at(i) == lValue2s.at(i)) {
            continue;
        }

        if (lValue1s.at(i) < lValue2s.at(i)) {
            return false;
        }

        if (lValue1s.at(i) > lValue2s.at(i)) {
            return true;
        }
    }

    return true;
}

bool QStringMath::operator <=(const QString other) { return (*this <= QStringMath(other)); }
bool QStringMath::operator <=(const QStringMath &other)
{
    QString lValue1s = mValue;
    QString lValue2s = other.mValue;

    standardizeStrings(mValue, other.mValue, lValue1s, lValue2s);

    for (int i = 0; i < lValue1s.size() - 1; i++) {
        if (lValue1s.at(i) == lValue2s.at(i)) {
            continue;
        }

        if (lValue1s.at(i) > lValue2s.at(i)) {
            return false;
        }

        if (lValue1s.at(i) < lValue2s.at(i)) {
            return true;
        }
    }

    return true;
}

bool QStringMath::operator !=(const QString other) { return (*this != QStringMath(other)); }
bool QStringMath::operator !=(const QStringMath &other)
{
    QString lValue1s = mValue;
    QString lValue2s = other.mValue;

    standardizeStrings(mValue, other.mValue, lValue1s, lValue2s);
    return lValue1s != lValue2s;
}

QStringMath &QStringMath::operator =(const QStringMath &other)
{
    mValue = other.mValue;
    mValue = toString();
    return *this;
}

bool QStringMath::operator ==(const QString other) { return (*this == QStringMath(other)); }
bool QStringMath::operator ==(const QStringMath &other)
{
    QString lValue1s = mValue;
    QString lValue2s = other.mValue;

    standardizeStrings(mValue, other.mValue, lValue1s, lValue2s);
    return lValue1s == lValue2s;
}

QString QStringMath::toString() {
    QString lValue = mValue;
    lValue.remove( QRegExp("^[0]*") );

    if (lValue.size() > 1 && lValue.contains('.')) {
        while (lValue.at(lValue.size() - 1) == '0' && lValue.size() > 1) {
            lValue.remove(lValue.size() - 1, 1);
        }
    }

    if (lValue.size() > 1) {
        if (lValue.at(lValue.size() - 1) == '.') {
            lValue.remove(lValue.size() - 1, 1);
        }
    }

    if (lValue == "." || lValue == "") {
        lValue = "0";
    }

    return lValue;
}

double QStringMath::toDouble() { return mValue.toDouble(); }
double QStringMath::toInt() { return mValue.toInt(); }

void QStringMath::standardizeStrings(const QString iValue1, const QString iValue2, QString &oValue1Out, QString &oValue2Out)
{
    QString lValue1 = iValue1;
    QString lValue2 = iValue2;

    if (!lValue1.contains(".")) { lValue1.append(".00"); }
    if (!lValue2.contains(".")) { lValue2.append(".00"); }

    while (lValue1.indexOf(".") < lValue2.indexOf(".")) { lValue1.prepend('0'); }
    while (lValue1.indexOf(".") > lValue2.indexOf(".")) { lValue2.prepend('0'); }
    while (lValue1.size() < lValue2.size()) { lValue1.append("0"); }
    while (lValue1.size() > lValue2.size()) { lValue2.append("0"); }

    oValue1Out = lValue1.prepend("0").append("0");
    oValue2Out = lValue2.prepend("0").append("0");
}
