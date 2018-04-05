#ifndef ENTITY_H
#define ENTITY_H

#include <QString>
#include <QList>
#include "utils.h"

class Entity
{
private:
    PersistenceType persistenceType;
    RepositoryType repositoryType;

public:
    Entity(PersistenceType persistenceType);
    Entity(PersistenceType persistenceType, RepositoryType repositoryType);
    PersistenceType getPersistenceType();
    RepositoryType getRepositoryType();
    QString getPersistenceTypeName();
    QString getRepositoryTypeName();
    virtual void updateAfterPersist(int id) = 0;
    virtual QString getSQLSelect(FetchMode fetchMode) = 0;
    virtual QString getSQLInsert() = 0;
    virtual QString getSQLUpdate(QString amount) = 0;
};

#endif // ENTITY_H
