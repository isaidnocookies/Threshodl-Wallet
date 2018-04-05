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
}

/**
 * Get persistence type for the entity. This is a hardcoded short cut
 * to not to deal with type reflection
 *
 * @brief Entity::getPersistenceType
 * @return
 */
PersistenceType Entity::getPersistenceType() {
    return this->persistenceType;
}

/**
 * Get a name for persitence types to ease debugging
 *
 * @brief Entity::getPersistenceTypeName
 * @return
 */
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
