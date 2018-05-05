#ifndef GRINDERINTERFACE_H
#define GRINDERINTERFACE_H

#include <QString>
#include <QStringList>

class GrinderInterface
{
public:
    virtual ~GrinderInterface() { }

    virtual QStringList grindValues(const QString iValue, const QString iCryptoCurrency = QStringLiteral("BTC")) = 0;
};

#endif // GRINDERINTERFACE_H
