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

/**
 * Save the Entity as a new instance in the storage object
 * This method should have the ability to save single isntances but also
 * collections or nested instances
 *
 * @brief Entity::save
 * @return bool indicating if the operation was successfull
 */
bool Entity::save() {
    bool result = false;
    WalletEntity* wallet;
    switch(this->persistenceType) {
        case PersistenceType::PERSISTENCE_TYPE_BILL:
            qDebug() << "Saving and instance of: " << this->getType() << " in " << this->getRepositoryType();
            result = true;
            break;
        case PersistenceType::PERSISTENCE_TYPE_WALLET:
            qDebug() << "Saving and instance of: " << this->getType() << " in " << this->getRepositoryType();
            wallet = (WalletEntity*)this;
            foreach (BillEntity* bill, wallet->getBills()) {
                bill->save();
            }
            result = true;
            break;
        default:
            result = false;
    }
    return result;
}

/**
 * Get all Entities matching the type of the caller. The usage could be in the form:
 * WalletEntity *entityReference = new WalletEntity();
 * QList<Entity*> list = entityReference->loadAll();
 *
 * @brief Entity::loadAll
 * @return QList<Entity*> a collection of Entity objects of the same type as the caller
 */
QList<Entity*> Entity::loadAll() {
    QList<Entity*> list;
    qDebug() << "Gettings all instances of: " << this->getType() << " in " << this->getRepositoryType();
    return list;
}

/**
 * Load an specific entity using the id in the parameter and the type of the caller
 * WalletEntity *entityReference = new WalletEntity();
 * Entity* entity = entityReference->load(get_index_somehow());
 *
 * @brief Entity::load
 * @param id
 * @return
 */
Entity* Entity::load(int id) {
    Entity* entity;
    switch(this->persistenceType) {
        case PersistenceType::PERSISTENCE_TYPE_BILL:
            entity = new BillEntity(NULL);
            break;
        case PersistenceType::PERSISTENCE_TYPE_WALLET:
            entity = new WalletEntity();
            break;
        default:
            entity = NULL;
    }
    qDebug() << "Gettings an instance of: " << this->getType() << " with id: " << id << " in " << this->getRepositoryType();
    return entity;
}

QString Entity::getType() {
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

QString Entity::getRepositoryType() {
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
