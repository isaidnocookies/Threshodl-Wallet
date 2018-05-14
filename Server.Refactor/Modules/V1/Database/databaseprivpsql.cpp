#include "databaseprivpsql.h"
#include "database.h"

#include <QString>
#include <QDebug>
#include <QRegExp>
#include <QMutex>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>

static QMutex       _connectionCounterLock;
static quint64      _connectionCounter      = 0;

QString DatabasePrivPSQL::_stringListToWhereIn(const QStringList iList)
{
    QString         lWhereIn;
    int             lCount      = iList.size();

    // Build where in
    for( int lIndex = 0; lIndex < lCount; lIndex++ )
    {
        if( lIndex != 0 ) lWhereIn = lWhereIn.append(QChar{','});
        QString lString = (iList.at(lIndex)).toLower();
        lWhereIn = lWhereIn.append(QStringLiteral("'%1'").arg(lString));
    }

    return lWhereIn;
}

QString DatabasePrivPSQL::_storageTableNameForAccount(const QString iAccountName)
{ return QStringLiteral("storage_%1").arg(sanatizedUsername(iAccountName)); }

QString DatabasePrivPSQL::_createStorageTableSqlQueryForAccount(const QString iAccountName)
{
    return QStringLiteral("CREATE TABLE %1 ( walletid text NOT NULL UNIQUE PRIMARY KEY, state integer, payload text );")
                    .arg(_storageTableNameForAccount(iAccountName));
}

bool DatabasePrivPSQL::_init()
{
    if( mApp ) {
        auto lConfig = mApp->configuration();

        if( lConfig->contains(QStringLiteral("DatabaseName")) )
            mDBName = lConfig->toString(QStringLiteral("DatabaseName"));
        else if( lConfig->contains(QStringLiteral("DBName")) )
            mDBName = lConfig->toString(QStringLiteral("DBName"));

        if( lConfig->contains(QStringLiteral("DatabaseHostName")) )
            mDBHostName = lConfig->toString(QStringLiteral("DatabaseHostName"));
        else if( lConfig->contains(QStringLiteral("DBHostName")) )
            mDBHostName = lConfig->toString(QStringLiteral("DBHostName"));

        if( lConfig->contains(QStringLiteral("DatabaseUserName")) )
            mDBUserName = lConfig->toString(QStringLiteral("DatabaseUserName"));
        else if( lConfig->contains(QStringLiteral("DBUserName")) )
            mDBUserName = lConfig->toString(QStringLiteral("DBUserName"));

        if( lConfig->contains(QStringLiteral("DatabasePassword")) )
            mDBPassword = lConfig->toString(QStringLiteral("DatabasePassword"));
        else if( lConfig->contains(QStringLiteral("DBPassword")) )
            mDBPassword = lConfig->toString(QStringLiteral("DBPassword"));

        if( lConfig->contains(QStringLiteral("DatabasePort")) )
            mDBPort = lConfig->toUInt16(QStringLiteral("DatabasePort"));
        else if( lConfig->contains(QStringLiteral("DBPort")) )
            mDBPort = lConfig->toUInt16(QStringLiteral("DBPort"));

        if( mDBPort < 1 )                                                           return false;
        if( mDBName.isEmpty() || mDBHostName.isEmpty() || mDBUserName.isEmpty() )   return false;

        // Now generate a unique database string
        _connectionCounterLock.lock();
        mInternalDatabaseString = QStringLiteral("DatabasePrivPSQL-1-Connection.%1").arg(_connectionCounter++);
        _connectionCounterLock.unlock();

        QSqlDatabase    lDB = QSqlDatabase::addDatabase(QStringLiteral("QPSQL"),mInternalDatabaseString);
        lDB.setDatabaseName(mDBName);
        lDB.setHostName(mDBHostName);
        lDB.setPort(mDBPort);
        lDB.setUserName(mDBUserName);
        lDB.setPassword(mDBPassword);
        return lDB.open();
    }

    return false;
}

bool DatabasePrivPSQL::_startTransactionAndLockTables(QSqlDatabase &iDatabase, const QStringList iTablesToLockForSharedMode, const QStringList iTablesToLockForExclusiveMode)
{
    if( iDatabase.transaction() ) {
        if( ! iTablesToLockForSharedMode.isEmpty() ) {
            for( QString lTN : iTablesToLockForSharedMode )
            {
                if( ! QSqlQuery{iDatabase}.exec( QStringLiteral("LOCK TABLE %1 IN SHARE MODE").arg(lTN) ) ) {
                    iDatabase.rollback();
                    return false;
                }
            }
        }

        if( ! iTablesToLockForExclusiveMode.isEmpty() ) {
            for( QString lTN : iTablesToLockForExclusiveMode )
            {
                if( ! QSqlQuery{iDatabase}.exec( QStringLiteral("LOCK TABLE %1 IN EXCLUSIVE MODE").arg(lTN) ) ) {
                    iDatabase.rollback();
                    return false;
                }
            }
        }

        return true;
    }

    return false;
}

bool DatabasePrivPSQL::_rollbackTransaction(QSqlDatabase &iDatabase)
{ return iDatabase.rollback(); }

bool DatabasePrivPSQL::_commitTransaction(QSqlDatabase &iDatabase)
{ return iDatabase.commit(); }

bool DatabasePrivPSQL::_startTransactionAndOpenAndLockTables(QSqlDatabase &oDatabase, const QStringList iTablesToLockForSharedMode, const QStringList iTablesToLockForExclusiveMode)
{
    oDatabase = QSqlDatabase::database(mInternalDatabaseString,true);
    return _startTransactionAndLockTables(oDatabase,iTablesToLockForSharedMode,iTablesToLockForExclusiveMode);
}

DatabasePrivPSQL::DatabasePrivPSQL()
{ }

QString DatabasePrivPSQL::sanatizedUsername(const QString iAccountName)
{ return mDatabase->sanatizedUsername(iAccountName); }

bool DatabasePrivPSQL::createTables()
{
    QSqlDatabase    lDB;

    if( _startTransactionAndOpen(lDB) ) {
        try {
            if( ! QSqlQuery{lDB}.exec( QStringLiteral("CREATE TABLE IF NOT EXISTS addresses( address text NOT NULL UNIQUE PRIMARY KEY, key text )") ) )
                throw __LINE__;

            if( ! QSqlQuery{lDB}.exec( QStringLiteral("CREATE TABLE IF NOT EXISTS addresses_atime( address text NOT NULL UNIQUE PRIMARY KEY, atime text )") ) )
                throw __LINE__;

            if( ! QSqlQuery{lDB}.exec( QStringLiteral("CREATE TABLE IF NOT EXISTS in_use_walletids( walletid text NOT NULL UNIQUE PRIMARY KEY )") ) )
                throw __LINE__;

            if( ! QSqlQuery{lDB}.exec( QStringLiteral("CREATE TABLE IF NOT EXISTS wallet_scratch_creation_area( walletid text NOT NULL UNIQUE PRIMARY KEY, payload text, owner text NOT NULL, state integer, ctime integer)") ) )
                throw __LINE__;

            if( ! QSqlQuery{lDB}.exec( QStringLiteral("CREATE TABLE IF NOT EXISTS free_walletid_prefix( walletidprefix integer PRIMARY KEY )") ) )
                throw __LINE__;

            // Insert '1' into free_walletid_prefix if it is empty
            QSqlQuery   lSelectFreeQuery{lDB};
            if( ! lSelectFreeQuery.exec( QStringLiteral("SELECT walletidprefix FROM free_walletid_prefix") ) || lSelectFreeQuery.numRowsAffected() < 1 ) {
                // Need to insert
                if( ! QSqlQuery{lDB}.exec( QStringLiteral("INSERT INTO free_walletid_prefix (walletidprefix) VALUES (1)") ) )
                    throw __LINE__;
            }

            return _commitTransaction(lDB);
        }catch(int iLine){
            qCritical() << "Unable to create table at line" << iLine;
        }
    }

    return false;
}

bool DatabasePrivPSQL::addressExists(const QString iAddress)
{
    QSqlDatabase        lDB;
    QString             lAddress = sanatizedUsername(iAddress);

    if( ! lAddress.isEmpty() && _startTransactionAndOpenAndLockTables(lDB, QStringList() << QStringLiteral("addresses")) ) {
        QSqlQuery   lQuery{lDB};
        lQuery.prepare( QStringLiteral("SELECT address FROM addresses WHERE address = :iAddress") );
        lQuery.bindValue( QStringLiteral(":iAddress"), lAddress );

        if( lQuery.exec() ) {
            int lAddressNo = lQuery.record().indexOf(QStringLiteral("address"));
            if( lQuery.first() ) {
                if( lQuery.value(lAddressNo).toString() == lAddress.toLower() ) {
                    _commitTransaction(lDB);
                    return true;
                } else {
                    // It does not exist, however we can not use it because there is a record using the name partially?
                    qWarning() << "The database returned a record for user" << iAddress << " (" << lAddress << "), however an internal string check returned that the username does not match what were looking for.";
                }
            }
        }

        _rollbackTransaction(lDB);
    }

    return false;
}

bool DatabasePrivPSQL::addressCreate(const QString iAddress, const QByteArray iPublicKey)
{
    QSqlDatabase        lDB;
    QString             lAddress = sanatizedUsername(iAddress);

    if( ! lAddress.isEmpty() && _startTransactionAndOpenAndLockTables(lDB, QStringList(), QStringList() << QStringLiteral("addresses")) ) {
        QSqlQuery   lQuery{lDB};
        lQuery.prepare( QStringLiteral("INSERT INTO addresses (address, key) VALUES (:iAddress,:iPublicKey)") );
        lQuery.bindValue( QStringLiteral(":iAddress"), lAddress );
        lQuery.bindValue( QStringLiteral(":iPublicKey"), QString::fromUtf8(iPublicKey) );

        if( lQuery.exec() && QSqlQuery{lDB}.exec(_createStorageTableSqlQueryForAccount(iAddress)) ) {
            return _commitTransaction(lDB);
        }

        _rollbackTransaction(lDB);
    }

    return false;
}

bool DatabasePrivPSQL::addressValidate(const QString iAddress, const QByteArray iPublicKey)
{
    QSqlDatabase        lDB;
    QString             lAddress = sanatizedUsername(iAddress);

    if( ! lAddress.isEmpty() && _startTransactionAndOpenAndLockTables(lDB, QStringList() << QStringLiteral("addresses")) ) {
        QSqlQuery   lQuery{lDB};
        lQuery.prepare( QStringLiteral("SELECT address, key FROM addresses WHERE address = :iAddress") );
        lQuery.bindValue( QStringLiteral(":iAddress"), lAddress );

        if( lQuery.exec() ) {
            int     lAddressNo  = lQuery.record().indexOf(QStringLiteral("address"));
            int     lKeyNo      = lQuery.record().indexOf(QStringLiteral("key"));

            if( lQuery.first() ) {
                if( lQuery.value(lAddressNo) == lAddress && lQuery.value(lKeyNo) == QString::fromUtf8(iPublicKey) ) {
                    _commitTransaction(lDB);
                    return true;
                } else {
                    // It does not exist, however we can not use it because there is a record using the name partially?
                    qWarning() << "The database returned a record for user" << iAddress << " (" << lAddress << "), however an internal string check returned that the username does not match what were looking for.";
                }
            }
        }

        _rollbackTransaction(lDB);
    }

    return false;
}

bool DatabasePrivPSQL::addressDelete(const QString iAddress)
{
    QSqlDatabase        lDB;
    QString             lAddress = sanatizedUsername(iAddress);

    if( ! lAddress.isEmpty() && _startTransactionAndOpenAndLockTables(lDB, QStringList(), QStringList() << QStringLiteral("addresses") << _storageTableNameForAccount(iAddress)) ) {
        QSqlQuery   lDeleteUserQuery(lDB);
        lDeleteUserQuery.prepare( QStringLiteral("DELETE FROM addresses WHERE address = :iAddress") );
        lDeleteUserQuery.bindValue( QStringLiteral(":iAddress"), lAddress );

        if( lDeleteUserQuery.exec() && QSqlQuery{lDB}.exec(QStringLiteral("DROP TABLE %1").arg(_storageTableNameForAccount(iAddress))) ) {
            return _commitTransaction(lDB);
        }

        _rollbackTransaction(lDB);
    }

    return false;
}

QByteArray DatabasePrivPSQL::publicKeyForAddress(const QString iAddress)
{
    QSqlDatabase        lDB;
    QByteArray          lResult;
    QString             lAddress = sanatizedUsername(iAddress);

    if( ! lAddress.isEmpty() && _startTransactionAndOpenAndLockTables(lDB, QStringList() << QStringLiteral("addresses")) ) {
        QSqlQuery   lQuery{lDB};
        lQuery.prepare( QStringLiteral("SELECT address, key FROM addresses WHERE address = :iAddress") );
        lQuery.bindValue( QStringLiteral(":iAddress"), lAddress );

        if( lQuery.exec() ) {
            int     lAddressNo  = lQuery.record().indexOf(QStringLiteral("address"));
            int     lKeyNo      = lQuery.record().indexOf(QStringLiteral("key"));

            if( lQuery.first() ) {
                if( lQuery.value(lAddressNo) == lAddress ) {
                    lResult = lQuery.value(lKeyNo).toString().toUtf8();
                } else {
                    // It does not exist, however we can not use it because there is a record using the name partially?
                    qWarning() << "The database returned a record for user" << iAddress << " (" << lAddress << "), however an internal string check returned that the username does not match what were looking for.";
                }
            }
        }

        if( lResult.isEmpty() )
            _rollbackTransaction(lDB);
        else
            _commitTransaction(lDB);
    }

    return lResult;
}

bool DatabasePrivPSQL::microWalletAcquireFreeWalletIdPrefixBlock(unsigned int iBlockSize, QString &oStartingWalletIdPrefix)
{
    QSqlDatabase        lDB;

    if( _startTransactionAndOpenAndLockTables(lDB, QStringList(), QStringList() << QStringLiteral("free_walletid_prefix") ) ) {
        QString         lSelectQueryString    = QStringLiteral("SELECT walletidprefix FROM free_walletid_prefix");
        QString         lUpdateQueryString;
        QSqlQuery       lSelectQuery{lDB};
        QSqlQuery       lUpdateQuery{lDB};

        if( lSelectQuery.exec(lSelectQueryString) && lSelectQuery.numRowsAffected() == 1 ) {
            int lWalletIdPrefixNo = lSelectQuery.record().indexOf(QStringLiteral("walletidprefix"));
            if( lSelectQuery.next() ) {
                quint16     lValue = lSelectQuery.value(lWalletIdPrefixNo).toULongLong();
                oStartingWalletIdPrefix = QString::number(lValue);
                lUpdateQueryString = QStringLiteral("UPDATE free_walletid_prefix SET walletidprefix = %1 WHERE walletidprefix = %2").arg(lValue + iBlockSize).arg(lValue);
                if( lUpdateQuery.exec(lUpdateQueryString) ) {
                    return _commitTransaction(lDB);
                }
            }
        }

        _rollbackTransaction(lDB);
    }

    return false;
}

bool DatabasePrivPSQL::microWalletsExists(const QStringList iMicroWalletIds)
{
    int                 lWalletIdCount  = iMicroWalletIds.size();
    QSqlDatabase        lDB;
    QString             lWhereIn        = _stringListToWhereIn(iMicroWalletIds);

    if( lWalletIdCount > 0 && _startTransactionAndOpenAndLockTables(lDB, QStringList() << QStringLiteral("addresses")) ) {
        // Build the query
        QString     lQueryString = QStringLiteral("SELECT walletid FROM in_use_walletids WHERE walletid IN (%1)").arg(lWhereIn);
        QSqlQuery   lQuery{lDB};
        if( lQuery.exec(lQueryString) ) {
            if( lQuery.numRowsAffected() == lWalletIdCount ) {
                return _commitTransaction(lDB);
            }
        }

        _rollbackTransaction(lDB);
    }
    return false;
}

bool DatabasePrivPSQL::microWalletsOwnershipCheck(const QStringList iMicroWalletIds, const QString iAddress)
{
    int                 lWalletIdCount  = iMicroWalletIds.size();
    QString             lTableName      = _storageTableNameForAccount(iAddress);
    QSqlDatabase        lDB;
    QString             lWhereIn        = _stringListToWhereIn(iMicroWalletIds);

    if( lWalletIdCount > 0 && _startTransactionAndOpenAndLockTables(lDB, QStringList() << lTableName) ) {
        // Build the query
        QString     lQueryString = QStringLiteral("SELECT walletid FROM %1 WHERE state = %2 AND walletid IN (%3)")
                .arg(lTableName)
                .arg(static_cast<int>(StorageRecordState::Unlocked),10)
                .arg(lWhereIn);
        QSqlQuery   lQuery{lDB};
        if( lQuery.exec(lQueryString) ) {
            if( lQuery.numRowsAffected() == lWalletIdCount ) {
                return _commitTransaction(lDB);
            }
        }

        _rollbackTransaction(lDB);
    }
    return false;
}

bool DatabasePrivPSQL::microWalletsChangeOwnership(const QStringList iMicroWalletIds, const QString iFromAddress, const QString iToAddress)
{
    if( iMicroWalletIds.isEmpty() || iFromAddress.isEmpty() || iToAddress.isEmpty() ) return false;

    if( iFromAddress == iToAddress ) return true;

    // Build our Queries before locking and doing things
    int             lWalletIdCount  = iMicroWalletIds.size();
    QSqlDatabase    lDB;
    QString         lFromTable      = _storageTableNameForAccount(iFromAddress);
    QString         lToTable        = _storageTableNameForAccount(iToAddress);
    QString         lWhereInString  = _stringListToWhereIn(iMicroWalletIds);

    QString         lUpdateStateInFROMQuery = QStringLiteral("UPDATE %1 SET state = %2 WHERE state = %3 AND walletid IN (%4)")
            .arg(lFromTable)
            .arg(static_cast<int>(StorageRecordState::Locked),10)
            .arg(static_cast<int>(StorageRecordState::Unlocked),10)
            .arg(lWhereInString);

    QString         lCopyRecordInTOQuery = QStringLiteral("INSERT INTO %1 (walletid, state, payload) SELECT walletid, state, payload FROM %2 WHERE state = %3 AND walletid IN(%4)")
            .arg(lToTable)
            .arg(lFromTable)
            .arg(static_cast<int>(StorageRecordState::Locked),10)
            .arg(lWhereInString);

    QString         lDeleteRecordInFROMQuery = QStringLiteral("DELETE FROM %1 WHERE state = %2 AND walletid IN(%3)")
            .arg(lFromTable)
            .arg(static_cast<int>(StorageRecordState::Locked),10)
            .arg(lWhereInString);

    QString         lUpdateStateInTOQuery = QStringLiteral("UPDATE %1 SET state = %2 WHERE state = %3 AND walletid IN (%4)")
            .arg(lToTable)
            .arg(static_cast<int>(StorageRecordState::Unlocked),10)
            .arg(static_cast<int>(StorageRecordState::Locked),10)
            .arg(lWhereInString);

    if( _startTransactionAndOpenAndLockTables(lDB, QStringList(), QStringList() << lFromTable << lToTable) ) {
        QSqlQuery   lUpdateStateInFROM(lDB);
        QSqlQuery   lCopyRecordInTO(lDB);
        QSqlQuery   lDeleteRecordInFROM(lDB);
        QSqlQuery   lUpdateStateInTO(lDB);

        if( lUpdateStateInFROM.exec(lUpdateStateInFROMQuery) && lUpdateStateInFROM.numRowsAffected() == lWalletIdCount ) {
            if( lCopyRecordInTO.exec(lCopyRecordInTOQuery) ) {
                if( lDeleteRecordInFROM.exec(lDeleteRecordInFROMQuery) && lDeleteRecordInFROM.numRowsAffected() == lWalletIdCount ) {
                    if( lUpdateStateInTO.exec(lUpdateStateInTOQuery) && lUpdateStateInTO.numRowsAffected() == lWalletIdCount ) {
                        return _commitTransaction(lDB);
                    }
                }
            }
        }
        _rollbackTransaction(lDB);
    }

    return false;
}

bool DatabasePrivPSQL::microWalletScratchCreates(const QMap<QString, QByteArray> iMicroWalletIdsAndPayloads, const QString iAddress, const quint64 iCreationTime)
{
    QSqlDatabase    lDB;
    QStringList     lWalletIds      = iMicroWalletIdsAndPayloads.keys();
    int             lWalletIdsCount = lWalletIds.size();
    QString         lAddress        = sanatizedUsername(iAddress);

    if( iMicroWalletIdsAndPayloads.isEmpty() || iAddress.isEmpty() ) return false;

    QString     lInUseQueryString   = QStringLiteral("INSERT INTO in_use_walletids (walletid) VALUES ");
    QString     lScratchQueryString = QStringLiteral("INSERT INTO wallet_scratch_creation_area (walletid,payload,owner,state,ctime) VALUES ");

    for( int lIndex = 0; lIndex < lWalletIdsCount; lIndex++ ) {
        if( lIndex != 0 ) {
            lScratchQueryString = lScratchQueryString.append(QStringLiteral(", "));
            lInUseQueryString   = lInUseQueryString.append(QStringLiteral(", "));
        }

        lInUseQueryString = lInUseQueryString.append(QStringLiteral("('%1')").arg(lWalletIds.at(lIndex)));
        lScratchQueryString = lScratchQueryString.append(
                    QStringLiteral("('%1','%2','%3',%4)")
                    .arg(lWalletIds.at(lIndex))
                    .arg(QString::fromUtf8(iMicroWalletIdsAndPayloads[lWalletIds.at(lIndex)].toBase64()))
                    .arg(lAddress)
                    .arg(static_cast<int>(StorageRecordState::Unlocked),10)
                    .arg(iCreationTime)
                    );
    }

    if( _startTransactionAndOpenAndLockTables(lDB, QStringList(), QStringList() << QStringLiteral("wallet_scratch_creation_area") << QStringLiteral("in_use_walletids")) ) {

        QSqlQuery   lInUseQuery{lDB};
        QSqlQuery   lScratchQuery{lDB};

        if( lInUseQuery.exec(lInUseQueryString) && lInUseQuery.numRowsAffected() == lWalletIdsCount && lScratchQuery.exec(lScratchQueryString) && lScratchQuery.numRowsAffected() == lWalletIdsCount ) {
            return _commitTransaction(lDB);
        }

        _rollbackTransaction(lDB);
    }

    return false;
}

bool DatabasePrivPSQL::microWalletMoveFromScratch(const QStringList iMicroWalletIds, const QString iAddress)
{
    QSqlDatabase    lDB;
    QString         lAddress        = sanatizedUsername(iAddress);
    QString         lTable          = _storageTableNameForAccount(iAddress);
    int             lWalletIdsCount = iMicroWalletIds.size();
    QString         lWhereInString  = _stringListToWhereIn(iMicroWalletIds);

    if( iMicroWalletIds.isEmpty() || iAddress.isEmpty() ) return false;

    QString     lMoveQueryString    = QStringLiteral("INSERT INTO %1 (walletid,state,payload) SELECT walletid,state,payload FROM wallet_scratch_creation_area WHERE owner = %2 AND walletid IN(%3)")
            .arg(lTable)
            .arg(lAddress)
            .arg(lWhereInString);

    QString     lDeleteQueryString  = QStringLiteral("DELETE FROM wallet_scratch_creation_area WHERE owner = %1 AND walletid IN(%2)")
            .arg(lAddress)
            .arg(lWhereInString);

    if( _startTransactionAndOpenAndLockTables(lDB, QStringList(), QStringList() << lTable << QStringLiteral("wallet_scratch_creation_area")) ) {

        QSqlQuery   lMoveQuery{lDB};
        QSqlQuery   lDeleteQuery{lDB};

        if( lMoveQuery.exec(lMoveQueryString) && lMoveQuery.numRowsAffected() == lWalletIdsCount && lDeleteQuery.exec(lDeleteQueryString) && lDeleteQuery.numRowsAffected() == lWalletIdsCount ) {
            return _commitTransaction(lDB);
        }

        _rollbackTransaction(lDB);
    }

    return false;

    // OLD WORKING:

//    QSqlDatabase    lDB;
//    QString         lTable          = _storageTableNameForAccount(iAddress);
//    QStringList     lWalletIds      = iMicroWalletIdsAndPayloads.keys();
//    int             lWalletIdsCount = lWalletIds.size();

//    if( iMicroWalletIdsAndPayloads.isEmpty() || iAddress.isEmpty() ) return false;

//    QString     lInUseQueryString   = QStringLiteral("INSERT INTO in_use_walletids (walletid) VALUES ");
//    QString     lStorageQueryString = QStringLiteral("INSERT INTO %1 (walletid,state,payload) VALUES ").arg(lTable);

//    for( int lIndex = 0; lIndex < lWalletIdsCount; lIndex++ ) {
//        if( lIndex != 0 ) {
//            lStorageQueryString = lStorageQueryString.append(QStringLiteral(", "));
//            lInUseQueryString   = lInUseQueryString.append(QStringLiteral(", "));
//        }

//        lInUseQueryString = lInUseQueryString.append(QStringLiteral("('%1')").arg(lWalletIds.at(lIndex)));
//        lStorageQueryString = lStorageQueryString.append(
//                    QStringLiteral("('%1',%2,'%3')")
//                    .arg(lWalletIds.at(lIndex))
//                    .arg(static_cast<int>(StorageRecordState::Unlocked),10)
//                    .arg(QString::fromUtf8(iMicroWalletIdsAndPayloads[lWalletIds.at(lIndex)].toBase64()))
//                    );
//    }

//    if( _startTransactionAndOpenAndLockTables(lDB, QStringList(), QStringList() << lTable << QStringLiteral("in_use_walletids")) ) {

//        QSqlQuery   lInUseQuery{lDB};
//        QSqlQuery   lStorageQuery{lDB};

//        if( lInUseQuery.exec(lInUseQueryString) && lInUseQuery.numRowsAffected() == lWalletIdsCount && lStorageQuery.exec(lStorageQueryString) && lStorageQuery.numRowsAffected() == lWalletIdsCount ) {
//            return _commitTransaction(lDB);
//        }

//        _rollbackTransaction(lDB);
//    }

//    return false;
}

QMap<QString, QByteArray> DatabasePrivPSQL::microWalletsCopyPayload(const QStringList iMicroWalletIds, const QString iAddress)
{
    QMap<QString, QByteArray>   lResult;
    if( iMicroWalletIds.isEmpty() || iAddress.isEmpty() ) return lResult;

    QSqlDatabase                lDB;
    QString                     lTable          = _storageTableNameForAccount(iAddress);
    QString                     lWhereIn        = _stringListToWhereIn(iMicroWalletIds);

    QString                     lSelectQueryString  = QStringLiteral("SELECT walletid,payload FROM %1 WHERE state = %2 AND walletid IN (%3)")
            .arg(lTable)
            .arg(static_cast<int>(StorageRecordState::Unlocked),10)
            .arg(lWhereIn);


    if( _startTransactionAndOpenAndLockTables(lDB,QStringList() << lTable) ) {
        QSqlQuery   lSelectQuery{lDB};

        if( lSelectQuery.exec(lSelectQueryString) ) {
            int lWalletIdNo = lSelectQuery.record().indexOf(QStringLiteral("walletid"));
            int lPayloadNo  = lSelectQuery.record().indexOf(QStringLiteral("payload"));
            while( ! lSelectQuery.next() ) {
                lResult[lSelectQuery.value(lWalletIdNo).toString()] = QByteArray::fromBase64(lSelectQuery.value(lPayloadNo).toString().toUtf8());
            }
        }

        _commitTransaction(lDB);
    }

    return lResult;
}

bool DatabasePrivPSQL::microWalletsDelete(const QStringList iMicroWalletIds, const QString iAddress)
{
    if( iMicroWalletIds.isEmpty() || iAddress.isEmpty() ) return false;

    QSqlDatabase    lDB;
    QString         lTable          = _storageTableNameForAccount(iAddress);
    int             lWalletIdsCount = iMicroWalletIds.size();
    QString         lWhereIn        = _stringListToWhereIn(iMicroWalletIds);

    QString     lInUseQueryString   = QStringLiteral("DELETE FROM in_use_walletids WHERE walletid IN (%1)").arg(lWhereIn);
    QString     lStorageQueryString = QStringLiteral("DELETE FROM %1 WHERE walletid IN (%2)").arg(lTable).arg(lWhereIn);

    if( _startTransactionAndOpenAndLockTables(lDB, QStringList(), QStringList() << lTable << QStringLiteral("in_use_walletids")) ) {

        QSqlQuery   lInUseQuery{lDB};
        QSqlQuery   lStorageQuery{lDB};

        if( lInUseQuery.exec(lInUseQueryString) && lInUseQuery.numRowsAffected() == lWalletIdsCount && lStorageQuery.exec(lStorageQueryString) && lStorageQuery.numRowsAffected() == lWalletIdsCount ) {
            return _commitTransaction(lDB);
        }

        _rollbackTransaction(lDB);
    }

    return false;
}
