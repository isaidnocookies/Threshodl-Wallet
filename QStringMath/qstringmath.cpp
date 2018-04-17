#include "qstringmath.h"

#include <math.h>

QStringMath::QStringMath() { mValue = "0.0"; }
QStringMath::QStringMath(const QString iString) { mValue = iString; }

QStringMath QStringMath::operator +(const QString other) { return (*this + QStringMath(other)); }
QStringMath QStringMath::operator +(const QStringMath &other)
{
    // ONLY WORKS FOR POSITIVE NUMBERS
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

    return QStringMath(lNewValue);
}

QStringMath QStringMath::operator -(const QString other) { return (*this - QStringMath(other)); }
QStringMath QStringMath::operator -(const QStringMath &other)
{
    QString lValue1s = mValue;
    QString lValue2s = other.mValue;

    standardizeStrings(mValue, other.mValue, lValue1s, lValue2s);

    return QStringMath(other.mValue);
}

bool QStringMath::operator <(const QString other) { return (*this < QStringMath(other)); }
bool QStringMath::operator <(const QStringMath &other)
{
    QString lValue1s = mValue;
    QString lValue2s = other.mValue;

    standardizeStrings(mValue, other.mValue, lValue1s, lValue2s);

    for (int i = 0; i < lValue1s.size(); i++) {
        if (lValue1s.at(i) < lValue2s.at(i)) {
            return true;
        }
    }

    return false;
}

bool QStringMath::operator >(const QString other) { return (*this > QStringMath(other)); }
bool QStringMath::operator >(const QStringMath &other)
{
    return (!(*this < other));
}

bool QStringMath::operator >=(const QString other) { return (*this >= QStringMath(other)); }
bool QStringMath::operator >=(const QStringMath &other)
{
    QString lValue1s = mValue;
    QString lValue2s = other.mValue;

    standardizeStrings(mValue, other.mValue, lValue1s, lValue2s);

    for (int i = 0; i < lValue1s.size(); i++) {
        if (lValue1s.at(i) >= lValue2s.at(i)) {
            return true;
        }
    }

    return false;
}

bool QStringMath::operator <=(const QString other) { return (*this <= QStringMath(other)); }
bool QStringMath::operator <=(const QStringMath &other)
{
    QString lValue1s = mValue;
    QString lValue2s = other.mValue;

    standardizeStrings(mValue, other.mValue, lValue1s, lValue2s);

    for (int i = 0; i < lValue1s.size(); i++) {
        if (lValue1s.at(i) <= lValue2s.at(i)) {
            return true;
        }
    }

    return false;
}

bool QStringMath::operator !=(const QString other) { return (this->mValue != other); }
bool QStringMath::operator !=(const QStringMath &other) { return (*this != other.mValue); }

bool QStringMath::operator ==(const QString other) { return (this->mValue == other); }
bool QStringMath::operator ==(const QStringMath &other) { return (this->mValue == other.mValue); }

QString QStringMath::toString() {
    return mValue;
}

double QStringMath::toDouble()
{
    return mValue.toDouble();
}

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
