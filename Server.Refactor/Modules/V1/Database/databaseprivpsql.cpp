#include "databaseprivpsql.h"

#include <QString>
#include <QDebug>
#include <QRegExp>
#include <QMutex>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>

static QMutex       _connectionCounterLock;
static quint64      _connectionCounter      = 0;

QString DataBasePrivPSQL::_stringListToWhereIn(const QStringList iList)
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

QString DataBasePrivPSQL::_sanitizeAccountName(const QString iAccountName)
{ return iAccountName.trimmed().split(QRegExp(QStringLiteral("\\W"))).join(QStringLiteral("_")).toLower(); }

QString DataBasePrivPSQL::_storageTableNameForAccount(const QString iAccountName)
{ return QStringLiteral("storage_%1").arg(_sanitizeAccountName(iAccountName)); }

QString DataBasePrivPSQL::_createStorageTableSqlQueryForAccount(const QString iAccountName)
{
    return QStringLiteral("CREATE TABLE %1 ( walletid text NOT NULL UNIQUE PRIMARY KEY, state integer, payload text );")
                    .arg(_storageTableNameForAccount(iAccountName));
}

bool DataBasePrivPSQL::_init()
{
    if( mApp ) {
        auto lConfig = mApp->configuration();

        if( lConfig->contains(QStringLiteral("DataBaseName")) )
            mDBName = lConfig->toString(QStringLiteral("DataBaseName"));
        else if( lConfig->contains(QStringLiteral("DBName")) )
            mDBName = lConfig->toString(QStringLiteral("DBName"));

        if( lConfig->contains(QStringLiteral("DataBaseHostName")) )
            mDBHostName = lConfig->toString(QStringLiteral("DataBaseHostName"));
        else if( lConfig->contains(QStringLiteral("DBHostName")) )
            mDBHostName = lConfig->toString(QStringLiteral("DBHostName"));

        if( lConfig->contains(QStringLiteral("DataBaseUserName")) )
            mDBUserName = lConfig->toString(QStringLiteral("DataBaseUserName"));
        else if( lConfig->contains(QStringLiteral("DBUserName")) )
            mDBUserName = lConfig->toString(QStringLiteral("DBUserName"));

        if( lConfig->contains(QStringLiteral("DataBasePassword")) )
            mDBPassword = lConfig->toString(QStringLiteral("DataBasePassword"));
        else if( lConfig->contains(QStringLiteral("DBPassword")) )
            mDBPassword = lConfig->toString(QStringLiteral("DBPassword"));

        if( lConfig->contains(QStringLiteral("DataBasePort")) )
            mDBPort = lConfig->toUInt16(QStringLiteral("DataBasePort"));
        else if( lConfig->contains(QStringLiteral("DBPort")) )
            mDBPort = lConfig->toUInt16(QStringLiteral("DBPort"));

        if( mDBPort < 1 )                                                           return false;
        if( mDBName.isEmpty() || mDBHostName.isEmpty() || mDBUserName.isEmpty() )   return false;

        // Now generate a unique database string
        _connectionCounterLock.lock();
        mInternalDataBaseString = QStringLiteral("DataBasePrivPSQL-1-Connection.%1").arg(_connectionCounter++);
        _connectionCounterLock.unlock();

        QSqlDatabase    lDB = QSqlDatabase::addDatabase(QStringLiteral("QPSQL"),mInternalDataBaseString);
        lDB.setDatabaseName(mDBName);
        lDB.setHostName(mDBHostName);
        lDB.setPort(mDBPort);
        lDB.setUserName(mDBUserName);
        lDB.setPassword(mDBPassword);
        return lDB.open();
    }

    return false;
}

bool DataBasePrivPSQL::_startTransactionAndLockTables(QSqlDatabase &iDataBase, const QStringList iTablesToLockForSharedMode, const QStringList iTablesToLockForExclusiveMode)
{
    if( iDataBase.transaction() ) {
        if( ! iTablesToLockForSharedMode.isEmpty() ) {
            for( QString lTN : iTablesToLockForSharedMode )
            {
                if( ! QSqlQuery{iDataBase}.exec( QStringLiteral("LOCK TABLE %1 IN SHARE MODE").arg(lTN) ) ) {
                    iDataBase.rollback();
                    return false;
                }
            }
        }

        if( ! iTablesToLockForExclusiveMode.isEmpty() ) {
            for( QString lTN : iTablesToLockForExclusiveMode )
            {
                if( ! QSqlQuery{iDataBase}.exec( QStringLiteral("LOCK TABLE %1 IN EXCLUSIVE MODE").arg(lTN) ) ) {
                    iDataBase.rollback();
                    return false;
                }
            }
        }

        return true;
    }

    return false;
}

bool DataBasePrivPSQL::_rollbackTransaction(QSqlDatabase &iDataBase)
{ return iDataBase.rollback(); }

bool DataBasePrivPSQL::_commitTransaction(QSqlDatabase &iDataBase)
{ return iDataBase.commit(); }

bool DataBasePrivPSQL::_startTransactionAndOpenAndLockTables(QSqlDatabase &oDatabase, const QStringList iTablesToLockForSharedMode, const QStringList iTablesToLockForExclusiveMode)
{
    oDatabase = QSqlDatabase::database(mInternalDataBaseString,true);
    return _startTransactionAndLockTables(oDatabase,iTablesToLockForSharedMode,iTablesToLockForExclusiveMode);
}

DataBasePrivPSQL::DataBasePrivPSQL()
{ }

bool DataBasePrivPSQL::createTables()
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

            return _commitTransaction(lDB);
        }catch(int iLine){
            qCritical() << "Unable to create table at line" << iLine;
        }
    }

    return false;
}

bool DataBasePrivPSQL::addressExists(const QString iAddress)
{
    QSqlDatabase        lDB;
    QString             lAddress = _sanitizeAccountName(iAddress);

    if( ! lAddress.isEmpty() && _startTransactionAndOpenAndLockTables(lDB, QStringList() << QStringLiteral("addresses")) ) {
        QSqlQuery   lQuery{lDB};
        lQuery.prepare( QStringLiteral("SELECT address FROM addresses WHERE address = :iAddress") );
        lQuery.bindValue( QStringLiteral(":iAddress"), lAddress );

        if( lQuery.exec() ) {
            int lAddressNo = lQuery.record().indexOf(QStringLiteral("address"));
            if( lQuery.first() ) {
                if( lQuery.value(lAddressNo).toString() == iAddress.toLower() ) {
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

bool DataBasePrivPSQL::addressCreate(const QString iAddress, const QByteArray iPublicKey)
{
    QSqlDatabase        lDB;
    QString             lAddress = _sanitizeAccountName(iAddress);

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

bool DataBasePrivPSQL::addressValidate(const QString iAddress, const QByteArray iPublicKey)
{
    QSqlDatabase        lDB;
    QString             lAddress = _sanitizeAccountName(iAddress);

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

bool DataBasePrivPSQL::addressDelete(const QString iAddress)
{
    QSqlDatabase        lDB;
    QString             lAddress = _sanitizeAccountName(iAddress);

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

QByteArray DataBasePrivPSQL::publicKeyForAddress(const QString iAddress)
{
    QSqlDatabase        lDB;
    QByteArray          lResult;
    QString             lAddress = _sanitizeAccountName(iAddress);

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

bool DataBasePrivPSQL::microWalletsExists(const QStringList iMicroWalletIds)
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

bool DataBasePrivPSQL::microWalletsOwnershipCheck(const QStringList iMicroWalletIds, const QString iAddress)
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

bool DataBasePrivPSQL::microWalletsChangeOwnership(const QStringList iMicroWalletIds, const QString iFromAddress, const QString iToAddress)
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

bool DataBasePrivPSQL::microWalletCreates(const QMap<QString, QByteArray> iMicroWalletIdsAndPayloads, const QString iAddress)
{
    QSqlDatabase    lDB;
    QString         lTable          = _storageTableNameForAccount(iAddress);
    QStringList     lWalletIds      = iMicroWalletIdsAndPayloads.keys();
    int             lWalletIdsCount = lWalletIds.size();

    if( iMicroWalletIdsAndPayloads.isEmpty() || iAddress.isEmpty() ) return false;

    QString     lInUseQueryString   = QStringLiteral("INSERT INTO in_use_walletids (walletid) VALUES ");
    QString     lStorageQueryString = QStringLiteral("INSERT INTO %1 (walletid,state,payload) VALUES ").arg(lTable);

    for( int lIndex = 0; lIndex < lWalletIdsCount; lIndex++ ) {
        if( lIndex != 0 ) {
            lStorageQueryString = lStorageQueryString.append(QStringLiteral(", "));
            lInUseQueryString   = lInUseQueryString.append(QStringLiteral(", "));
        }

        lInUseQueryString = lInUseQueryString.append(QStringLiteral("('%1')").arg(lWalletIds.at(lIndex)));
        lStorageQueryString = lStorageQueryString.append(
                    QStringLiteral("('%1',%2,'%3')")
                    .arg(lWalletIds.at(lIndex))
                    .arg(static_cast<int>(StorageRecordState::Unlocked),10)
                    .arg(QString::fromUtf8(iMicroWalletIdsAndPayloads[lWalletIds.at(lIndex)].toBase64()))
                    );
    }

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

QMap<QString, QByteArray> DataBasePrivPSQL::microWalletsCopyPayload(const QStringList iMicroWalletIds, const QString iAddress)
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

bool DataBasePrivPSQL::microWalletsDelete(const QStringList iMicroWalletIds, const QString iAddress)
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
