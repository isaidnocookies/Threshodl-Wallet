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
    QString getType();
    QString getRepositoryType();

public:
    Entity(PersistenceType persistenceType);
    Entity(PersistenceType persistenceType, RepositoryType repositoryType);
    bool save();
    QList<Entity*> loadAll();
    Entity* load(int id);
};

#endif // ENTITY_H
