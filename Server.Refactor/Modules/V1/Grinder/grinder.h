#ifndef GRINDER_H
#define GRINDER_H

#include "../../Interface/GrinderInterface/grinderinterface.h"

#include "qstringmath.h"

#include <QString>
#include <QStringList>
#include <QMap>
#include <QSharedPointer>

class App;
class GrinderML;
class Grinder : public GrinderInterface
{
    friend class GrinderML;
public:
    Grinder();
    virtual ~Grinder();

    QStringList grindValues(const QString iValue, const QString iCryptoCurrency = QStringLiteral("BTC")) override;

protected:
    typedef struct {
        QStringMath             Value;
        int                     NumberOfPasses;
    } PassesEntry_t;

    typedef QSharedPointer<PassesEntry_t>           PassesEntryRef;

    typedef struct {
        QString                 Currency;
        int                     DefaultPasses;
        int                     MaxWalletsInt;
        QList<PassesEntryRef>   CurrencyPasses;
    } GrindingConstraints_t;

    typedef QSharedPointer<GrindingConstraints_t>   GrindingConstraintsRef;

    App *                                           mApp                    = nullptr;
    QMap< QString, GrindingConstraintsRef >         mGrindingConstraints;

    bool _normalizeString(const QString iInput, QString &oOutput, bool &oIsNegative);
    QString _divideStringValue(const QString iValue, unsigned int iDivsor);
    QStringList _appendQStringLists(const QStringList iFirst, const QStringList iSecond);
    QStringList _sortNumberedQStringList(const QStringList iList, bool iHighestFirst = true);
    QStringList _grindValueOnePass(const QString iValue, GrindingConstraintsRef iConstraints);

};

class GrinderML
{
public:
    GrinderML();
    static void * creator(void * pointerToAppObject);                                      // Returns a pointer to a new object
    static bool doInit(void * pointerToThis, void * pointerToAppObject);                   // Returns true on DoInit success
    static bool startInOwnThread();                                                        // Returns true if should be created and started in own thread
    static bool start(void * pointerToThis, void * pointerToAppObject);                    // Returns true on Start success
};

#endif // GRINDER_H
