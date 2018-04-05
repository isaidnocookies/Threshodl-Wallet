#ifndef ENTITY_H
#define ENTITY_H

#include <QString>
#include <QObject>
#include <QList>
#include "utils.h"

class Entity : public QObject
{
private:
    PersistenceType persistenceType;

public:
    Entity(PersistenceType persistenceType);
    PersistenceType getPersistenceType();
    QString getPersistenceTypeName();
    virtual void updateAfterPersist(int id) = 0;
    virtual QString getSQLSelect(FetchMode fetchMode) = 0;
    virtual QString getSQLInsert() = 0;
    virtual QString getSQLUpdate(QString amount) = 0;
};

#endif // ENTITY_H
