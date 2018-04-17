#ifndef QSTRINGMATH_H
#define QSTRINGMATH_H

#include <QString>

class QStringMath
{
public:
    QStringMath();
    QStringMath(const QString iString);

    QStringMath operator +(const QString other);
    QStringMath operator +(const QStringMath &other);

    QStringMath operator -(const QString other);
    QStringMath operator -(const QStringMath &other);

    bool operator <(const QString other);
    bool operator <(const QStringMath &other);

    bool operator >(const QString other);
    bool operator >(const QStringMath &other);

    bool operator <=(const QString other);
    bool operator <=(const QStringMath &other);

    bool operator >=(const QString other);
    bool operator >=(const QStringMath &other);

    bool operator ==(const QString other);
    bool operator ==(const QStringMath &other);

    bool operator !=(const QString other);
    bool operator !=(const QStringMath &other);

    QString toString();
    double toDouble();

    static void standardizeStrings(const QString iValue1, const QString iValue2, QString &oValue1Out, QString &oValue2Out);

private:
    QString mValue;
};

#endif // QSTRINGMATH_H
