#include "sqliteinterface.h"

#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include "utils.h"
#include "walletentity.h"
#include "billentity.h"

SQLiteInterface::SQLiteInterface(const QString& path) {
    this->mDataBase = QSqlDatabase::addDatabase("QSQLITE");
    this->mDataBase.setDatabaseName(path);

    if (!this->mDataBase.open()) {
       qDebug() << "Error: connection with database fail";
    } else {
       qDebug() << "Database: connection ok";
    }
}

bool SQLiteInterface::insert(Entity* entity, QSqlQuery preparedQuery) {
    bool error = false;
    QString query = entity->getSQLInsert();
    //qDebug() << "trying with: " << query;
    // preparedQuery.prepare(query);
    if(preparedQuery.exec(query)) {
        entity->updateAfterPersist(preparedQuery.lastInsertId().toInt());
        qDebug() << entity->getPersistenceTypeName() << " successfully stored";
    } else {
        error = true;
        qDebug() << "Error saving instance of:  " << entity->getPersistenceTypeName() << ". " << preparedQuery.lastError();
    }
    return !error;
}

QList<QSqlRecord> SQLiteInterface::selectAll(Entity* entity, FetchMode fetchMode) {
    QSqlQuery preparedQuery;
    QList<QSqlRecord> result;
    QString query = entity->getSQLSelect(fetchMode);
    preparedQuery.prepare(query);
    if(preparedQuery.exec()) {
        while(preparedQuery.next()) {
            result.append(preparedQuery.record());
        }
    }
    return result;
}

bool SQLiteInterface::persist(Entity* entity) {
    bool error = false;
    QSqlQuery preparedQuery;
    WalletEntity* wallet;
    this->mDataBase.transaction();
    switch(entity->getPersistenceType()) {
        case PersistenceType::PERSISTENCE_TYPE_BILL:
            if(this->insert(entity, preparedQuery)) {
                this->mDataBase.commit();
            } else {
                this->mDataBase.rollback();
                qDebug() << "Error saving instance of:  " << entity->getPersistenceTypeName() << ". " << preparedQuery.lastError();
            }
        break;
        case PersistenceType::PERSISTENCE_TYPE_WALLET:
            wallet = (WalletEntity*)entity;
            if(this->insert(entity, preparedQuery)) {
                if (wallet->getBills().empty()) {
                    this->mDataBase.commit();
                } else {
                    foreach(BillEntity* bill, wallet->getBills()) {
                        if(!this->insert(bill, preparedQuery)) {
                            this->mDataBase.rollback();
                            error = true;
                            break;
                        }
                    }
                    if (!error) {
                        this->mDataBase.commit();
                    }
                }
            } else {
                this->mDataBase.rollback();
                qDebug() << "Error saving instance of:  " << entity->getPersistenceTypeName() << ". " << preparedQuery.lastError();
            }
        break;
        default:
            qDebug() << "Unknown entity type";
            error = true;
    }
    return !error;
}

QList<Entity*> SQLiteInterface::findAll(PersistenceType persistenceType) {
    QList<Entity*> result;
    QList<QSqlRecord> queryResults;
    WalletEntity* wallet = new WalletEntity();
    BillEntity* bill = new BillEntity(wallet);
    switch (persistenceType) {
        case PersistenceType::PERSISTENCE_TYPE_BILL:
            // Using eager for Bills since each one belongs to just one wallet
            queryResults = this->selectAll(bill, FetchMode::FETCH_MODE_EAGER);
            foreach (QSqlRecord record, queryResults) {
                WalletEntity* wallet = new WalletEntity(record);
                Entity* entity = new BillEntity(record, wallet);
                result.append(entity);
            }
            break;
        case PersistenceType::PERSISTENCE_TYPE_WALLET:
            // Usong lazy for Wallets since the bills can be a large amount
            queryResults = this->selectAll(wallet, FetchMode::FETCH_MODE_LAZY);
            foreach (QSqlRecord record, queryResults) {
                Entity* entity = new WalletEntity(record);
                result.append(entity);
            }
            break;
    }
    return result;
}
