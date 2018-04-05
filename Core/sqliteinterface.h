#ifndef SQLITEINTERFACE_H
#define SQLITEINTERFACE_H

#include <QString>
#include <QList>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include "entity.h"

class SQLiteInterface
{
private:
    QSqlDatabase mDataBase;
    bool insert(Entity* entity, QSqlQuery preparedQuery);
    QList<QSqlRecord> selectAll(Entity* entity, FetchMode fetchMode);

public:
    SQLiteInterface(const QString& path);
    bool persist(Entity* entity);
    QList<Entity*> findAll(PersistenceType persistenceType);
};

#endif // SQLITEINTERFACE_H
