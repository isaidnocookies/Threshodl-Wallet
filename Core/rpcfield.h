#ifndef RPCFIELD_H
#define RPCFIELD_H

#include <QString>
#include <QVariant>

class RPCField
{
public:
    QString     Key;
    QVariant    Value;

    RPCField(const QString iKey, const QVariant iValue) : Key(iKey), Value(iValue) { }
    RPCField(const RPCField &iOther) : Key(iOther.Key), Value(iOther.Value) { }

    RPCField &operator=(const RPCField &iOther) { Key = iOther.Key; Value = iOther.Value; return *this; }

    QString key() const { return Key; }
    QVariant value() const { return Value; }
};

#endif // RPCFIELD_H
