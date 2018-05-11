#ifndef DATABASEPRIVPSQL_H
#define DATABASEPRIVPSQL_H

#include "databasepriv.h"
#include "app.h"

#include <QSqlDatabase>

class Database;
class DatabasePrivPSQL : public DatabasePriv
{
    friend class Database;
protected:
    enum StorageRecordState {
        Invalid     = 0x0,
        Unlocked    = 0x1,
        Locked      = 0x2
    };

    typedef enum StorageRecordState StorageRecordState;

    static QString _stringListToWhereIn(const QStringList iList);

    QString _sanitizeAccountName(const QString iAccountName);
    QString _storageTableNameForAccount(const QString iAccountName);
    QString _createStorageTableSqlQueryForAccount(const QString iAccountName);

    App *               mApp            = nullptr;
    QString             mDBUserName;
    QString             mDBPassword;
    QString             mDBName;
    QString             mDBHostName;
    quint16             mDBPort         = 5432;

    QString             mInternalDatabaseString;

    bool    _init();

    bool    _startTransactionAndLockTables(QSqlDatabase &iDatabase, const QStringList iTablesToLockForSharedMode = QStringList(), const QStringList iTablesToLockForExclusiveMode = QStringList());
    bool    _startTransaction(QSqlDatabase &iDatabase)      { return _startTransactionAndLockTables(iDatabase, QStringList(), QStringList()); }
    bool    _rollbackTransaction(QSqlDatabase &iDatabase);
    bool    _commitTransaction(QSqlDatabase &iDatabase);

    bool    _startTransactionAndOpenAndLockTables(QSqlDatabase &oDatabase, const QStringList iTablesToLockForSharedMode = QStringList(), const QStringList iTablesToLockForExclusiveMode = QStringList());
    bool    _startTransactionAndOpen(QSqlDatabase &oDatabase)      { return _startTransactionAndOpenAndLockTables(oDatabase,QStringList(), QStringList()); }

public:
    DatabasePrivPSQL();

    bool                        createTables() override;
    bool                        addressExists( const QString iAddress ) override;
    bool                        addressCreate( const QString iAddress, const QByteArray iPublicKey ) override;
    bool                        addressValidate( const QString iAddress, const QByteArray iPublicKey ) override;
    bool                        addressDelete( const QString iAddress ) override;
    QByteArray                  publicKeyForAddress(const QString iAddress) override;
    bool                        microWalletAcquireFreeWalletIdPrefixBlock(unsigned int iBlockSize, QString &oStartingWalletIdPrefix) override;
    bool                        microWalletsExists( const QStringList iMicroWalletIds ) override;
    bool                        microWalletsOwnershipCheck( const QStringList iMicroWalletIds, const QString iAddress ) override;
    bool                        microWalletsChangeOwnership( const QStringList iMicroWalletIds, const QString iFromAddress, const QString iToAddress ) override;
    bool                        microWalletScratchCreates( const QMap< QString, QByteArray> iMicroWalletIdsAndPayloads, const QString iAddress, const quint64 iCreationTime ) override;
    bool                        microWalletMoveFromScratch(const QStringList iMicroWalletIds, const QString iAddress ) override;
    QMap< QString, QByteArray>  microWalletsCopyPayload( const QStringList iMicroWalletIds, const QString iAddress ) override;
    bool                        microWalletsDelete( const QStringList iMicroWalletIds, const QString iAddress ) override;
};

#endif // DATABASEPRIVPSQL_H
