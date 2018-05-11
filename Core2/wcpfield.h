#ifndef WCPFIELD_H
#define WCPFIELD_H

#include <QString>
#include <QVariant>

class WCPField
{
public:
    QString     Key;
    QVariant    Value;

    WCPField(const QString iKey, const QVariant iValue) : Key(iKey), Value(iValue) { }
    WCPField(const WCPField &iOther) : Key(iOther.Key), Value(iOther.Value) { }

    WCPField &operator=(const WCPField &iOther) { Key = iOther.Key; Value = iOther.Value; return *this; }

    QString key() const { return Key; }
    QVariant value() const { return Value; }
};

#endif // WCPFIELD_H
