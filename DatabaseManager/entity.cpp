#include "entity.h"

#include <QDebug>
#include "billentity.h"
#include "walletentity.h"

/**
 * Initialize a persistable Entity using the defined type
 * Indicating the type is a shortcut to avoid using reflections
 * should work as lonf as the models remain simple enough.
 * Entity objects are not abstract and instead they hold all
 * persistence logic.
 * This way every derived class can just select how to be persisted
 * By default, the implementation uses QSettings for value storage
 *
 * @brief Entity::Entity
 * @param persistenceType
 */
Entity::Entity(PersistenceType persistenceType) {
    this->persistenceType = persistenceType;
    this->repositoryType = RepositoryType::REPOSITORY_TYPE_QSETTINGS;
}

/**
 * Initialize a persistable Entity using the defined type
 * Indicating the type is a shortcut to avoid using reflections
 * should work as lonf as the models remain simple enough.
 * Entity objects are not abstract and instead they hold all
 * persistence logic.
 * This way every derived class can just select how to be persisted
 * using repositoryType parameter
 *
 * @brief Entity::Entity
 * @param persistenceType
 * @param repositoryType
 */
Entity::Entity(PersistenceType persistenceType, RepositoryType repositoryType) {
    this->persistenceType = persistenceType;
    this->repositoryType = repositoryType;
}

PersistenceType Entity::getPersistenceType() {
    return this->persistenceType;
}

RepositoryType Entity::getRepositoryType() {
    return this->repositoryType;
}

QString Entity::getPersistenceTypeName() {
    QString result = "unknown";
    switch(this->persistenceType) {
        case PersistenceType::PERSISTENCE_TYPE_BILL:
            result = "bill";
            break;
        case PersistenceType::PERSISTENCE_TYPE_WALLET:
            result = "wallet";
            break;
    }
    return result;
}

QString Entity::getRepositoryTypeName() {
    QString result = "unknown";
    switch(this->repositoryType) {
        case RepositoryType::REPOSITORY_TYPE_QSETTINGS:
            result = "qsettings";
            break;
        case RepositoryType::REPOSITORY_TYPE_SQLITE:
            result = "sqlite";
            break;
    }
    return result;
}
