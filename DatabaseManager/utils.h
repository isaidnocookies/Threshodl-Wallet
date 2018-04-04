#ifndef TYPES_H
#define TYPES_H
/**
 * Standardize wallet type or mode management
 *
 * @brief The WalletType enum
 */
enum class WalletType
  {
     WALLET_TYPE_LIGHT = 0,
     WALLET_TYPE_DARK = 1
  };
/**
 * Easy workarround for persistent classes typecasting
 *
 * @brief The WalletMode enum
 */
enum class PersistenceType
  {
     PERSISTENCE_TYPE_WALLET = 0,
     PERSISTENCE_TYPE_BILL = 1
  };
/**
 * Repository type for types persistence
 *
 * @brief The RepositoryType enum
 */
enum class RepositoryType
  {
     REPOSITORY_TYPE_QSETTINGS = 0,
     REPOSITORY_TYPE_SQLITE = 1
  };
#endif // TYPES_H
