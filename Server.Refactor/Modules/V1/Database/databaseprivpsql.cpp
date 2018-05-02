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

QString DataBasePrivPSQL::_sanitizeAccountName(const QString iAccountName)
{ return iAccountName.trimmed().split(QRegExp(QStringLiteral("\\W"))).join(QStringLiteral("_")).toLower(); }

QString DataBasePrivPSQL::_escrowTableNameForAccount(const QString iAccountName)
{ return QStringLiteral("escrow_%1").arg(_sanitizeAccountName(iAccountName)); }

QString DataBasePrivPSQL::_createEscrowTableSqlQueryForAccount(const QString iAccountName)
{
    return QStringLiteral("CREATE TABLE %1 ( walletid text NOT NULL UNIQUE PRIMARY KEY, state integer, payload text );")
                    .arg(_escrowTableNameForAccount(iAccountName));
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

            for( QString lTN : iTablesToLockForExclusiveMode )
            {
                if( ! QSqlQuery{iDataBase}.exec( QStringLiteral("LOCK TABLE %1 IN EXCLUSIVE MODE").arg(lTN) ) ) {
                    iDataBase.rollback();
                    return false;
                }
            }

            return true;
        }
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

        if( lQuery.exec() && QSqlQuery{lDB}.exec(_createEscrowTableSqlQueryForAccount(iAddress)) ) {
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

    if( ! lAddress.isEmpty() && _startTransactionAndOpenAndLockTables(lDB, QStringList(), QStringList() << QStringLiteral("addresses") << _escrowTableNameForAccount(iAddress)) ) {
        QSqlQuery   lDeleteUserQuery(lDB);
        lDeleteUserQuery.prepare( QStringLiteral("DELETE FROM addresses WHERE address = :iAddress") );
        lDeleteUserQuery.bindValue( QStringLiteral(":iAddress"), lAddress );

        if( lDeleteUserQuery.exec() && QSqlQuery{lDB}.exec(QStringLiteral("DROP TABLE %1").arg(_escrowTableNameForAccount(iAddress))) ) {
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

    if( lWalletIdCount > 0 && _startTransactionAndOpenAndLockTables(lDB, QStringList() << QStringLiteral("addresses")) ) {
        // Build the query
        QString     lQueryString = QStringLiteral("SELECT walletid FROM in_use_walletids WHERE walletid IN (");

        for( int lIndex = 0; lIndex < lWalletIdCount; lIndex++ ) {
            if( lIndex != 0 ) lQueryString = lQueryString.append(QChar{','});
            lQueryString = lQueryString.append(QChar{'\''});
            lQueryString = lQueryString.append(iMicroWalletIds.at(lIndex));
            lQueryString = lQueryString.append(QChar{'\''});
        }
        lQueryString = lQueryString.append(QChar{')'});

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
    QString             lTableName      = _escrowTableNameForAccount(iAddress);
    QSqlDatabase        lDB;

    if( lWalletIdCount > 0 && _startTransactionAndOpenAndLockTables(lDB, QStringList() << lTableName) ) {
        // Build the query
        QString     lQueryString = QStringLiteral("SELECT walletid FROM %1 WHERE state = %2 AND walletid IN (").arg(lTableName).arg(static_cast<EscrowRecordState::Unlocked>);

        for( int lIndex = 0; lIndex < lWalletIdCount; lIndex++ ) {
            if( lIndex != 0 ) lQueryString = lQueryString.append(QChar{','});
            lQueryString = lQueryString.append(QChar{'\''});
            lQueryString = lQueryString.append(iMicroWalletIds.at(lIndex));
            lQueryString = lQueryString.append(QChar{'\''});
        }
        lQueryString = lQueryString.append(QChar{')'});

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

}

bool DataBasePrivPSQL::microWalletCreates(const QMap<QString, QByteArray> iMicroWalletIdsAndPayloads, const QString iAddress)
{

}

QMap<QString, QByteArray> DataBasePrivPSQL::microWalletsCopyPayload(const QStringList iMicroWalletIds, const QString iAddress)
{

}

bool DataBasePrivPSQL::microWalletsDelete(const QStringList iMicroWalletIds, const QString iAddress)
{

}
