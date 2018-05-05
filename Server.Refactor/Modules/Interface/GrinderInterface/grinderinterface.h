#ifndef GRINDERINTERFACE_H
#define GRINDERINTERFACE_H

#include <QString>
#include <QStringList>

class Grinder
{
public:
    virtual ~Grinder() { }

    virtual QStringList grindValues(const QString iValue, const QString iCryptoCurrency = QStringLiteral("BTC")) = 0;
};

#endif // GRINDERINTERFACE_H
