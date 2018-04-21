#include "dbinterfacev1.h"

#include <QDebug>
#include <QStringList>
#include <QRegExp>
#include <QVariant>
#include <QSqlDatabase>
#include <QSqlRecord>
#include <QSqlQuery>

bool DBInterfaceV1::_connected()
{
    QMutexLocker l{&mConnectionCounterLock};
    return (mConnectionCounter != 0);
}

bool DBInterfaceV1::_connectOrReconnectToDB(QSqlDatabase &oDatabase)
{
    if( _connected() ) {
        return _reconnectToDB(lastConnectionCounterString(),oDatabase);
    }

    QString lCounter;
    return _connectToDB(lCounter,oDatabase);
}

bool DBInterfaceV1::_connectToDB(QString &oConnectionCounterString, QSqlDatabase &oDatabase)
{
    oConnectionCounterString = _nextConnectionCounterString();
    oDatabase = QSqlDatabase::addDatabase(mSqlType,oConnectionCounterString);
    oDatabase.setHostName(mHostName);
    oDatabase.setDatabaseName(mDatabaseName);
    oDatabase.setUserName(mUserName);
    oDatabase.setPassword(mPassword);
    return oDatabase.open();
}

bool DBInterfaceV1::_reconnectToDB(const QString iConnectionCounterString, QSqlDatabase &oDatabase)
{
    oDatabase = QSqlDatabase::database(iConnectionCounterString,true);
    return oDatabase.isOpen();
}

bool DBInterfaceV1::_beginTransaction(QSqlDatabase &iDatabase)
{ return iDatabase.transaction(); }

bool DBInterfaceV1::_beginTransactionLockTable(QSqlDatabase &iDatabase, const QString iTableName, bool iLockExclusive)
{
    if( _beginTransaction(iDatabase) ) {
        if(
            mSqlType != QStringLiteral("QPSQL") ||
            QSqlQuery(
                    QStringLiteral( "LOCK TABLE %1 IN %2 MODE" )
                    .arg(iTableName)
                    .arg( iLockExclusive ? QStringLiteral("EXCLUSIVE") : QStringLiteral("SHARE") )
                    ).exec()
        ) {
            return true;
        }else{
            // Need to close the transaction and abort the lock;
            _commitTransaction(iDatabase);
        }
    }
    return false;
}

bool DBInterfaceV1::_beginTransactionLockTables(QSqlDatabase &iDatabase, const QStringList iTableNames, bool iLockExclusive)
{
    if( _beginTransaction(iDatabase) ) {
        if( mSqlType != QStringLiteral("QPSQL") ) return true;

        for( QString lTN : iTableNames ) {
            if( ! QSqlQuery(
                        QStringLiteral( "LOCK TABLE %1 IN %2 MODE" )
                        .arg( lTN )
                        .arg( iLockExclusive ? QStringLiteral("EXCLUSIVE") : QStringLiteral("SHARE") )
                        ).exec()
            ) {
                // Need to close the transaction and abort the lock;
                _commitTransaction(iDatabase);
                return false;
            }
        }

        return true;
    }
    return false;
}

bool DBInterfaceV1::_commitTransaction(QSqlDatabase &iDatabase)
{ return iDatabase.commit(); }

bool DBInterfaceV1::_commitTransactionUnlockTable(QSqlDatabase &iDatabase, const QString iTableName)
{ Q_UNUSED(iTableName) return _commitTransaction(iDatabase); }

bool DBInterfaceV1::_commitTransactionUnlockTables(QSqlDatabase &iDatabase, const QStringList iTableNames)
{ Q_UNUSED(iTableNames) return _commitTransaction(iDatabase); }

bool DBInterfaceV1::_rollbackTransaction(QSqlDatabase &iDatabase)
{ return iDatabase.rollback(); }

void DBInterfaceV1::_closeDB(const QString iConnectionCounterString, QSqlDatabase &iDatabase)
{
    iDatabase.commit();
    iDatabase.close();
    QSqlDatabase::removeDatabase(iConnectionCounterString);
}

QString DBInterfaceV1::lastConnectionCounterString() const
{ return mLastConnectionCounterString; }

QString DBInterfaceV1::_sanitizeAccountName(const QString iAccountName)
{ return iAccountName.trimmed().split(QRegExp(QStringLiteral("\\W"))).join(QStringLiteral("_")).toLower(); }

QString DBInterfaceV1::_escrowTableNameForAccount(const QString iAccountName)
{ return QStringLiteral("escrow_%1").arg(_sanitizeAccountName(iAccountName)); }

QSqlQuery DBInterfaceV1::_createEscrowTableSqlQueryForAccount(const QString iAccountName, QSqlDatabase iDatabase)
{
    return QSqlQuery(
                QStringLiteral("CREATE TABLE %1( rid integer PRIMARY KEY, walletid text NOT NULL UNIQUE, owner text NOT NULL, payload blob )")
                .arg(_escrowTableNameForAccount(iAccountName)),
                iDatabase
                );
}

DBInterfaceV1::DBInterfaceV1(const QString iUserName, const QString iPassword, const QString iDatabaseName, const QString iHostName, quint16 iPort)
    : DBInterface()
    , mUserName(iUserName), mPassword(iPassword)
    , mDatabaseName(iDatabaseName), mHostName(iHostName)
    , mPort(iPort)
{

}

void DBInterfaceV1::setSqlType(const QString iSqlType)
{ mSqlType = iSqlType; }

QString DBInterfaceV1::sqlType() const
{ return mSqlType; }

bool DBInterfaceV1::initDB()
{
    QSqlDatabase    lDB;
    bool            lRet            = false;

    if( _connectOrReconnectToDB(lDB) ) {
        if( _beginTransaction(lDB) ) {
            // This might be the correct format for PSQL but does not work on SQLITE, keep for the future:
            // "CREATE TABLE IF NOT EXISTS addresses( rid integer PRIMARY KEY, address text NOT NULL, key blob, UNIQUE(rid, address) )"
            try {
                QSqlQuery   lQuery(lDB);
                if( ! lQuery.exec( QStringLiteral("CREATE TABLE IF NOT EXISTS addresses( rid integer PRIMARY KEY, address text NOT NULL UNIQUE, key blob )") ) )
                    throw __LINE__;

                if( ! lQuery.exec( QStringLiteral("CREATE TABLE IF NOT EXISTS in_use_walletids( rid integer PRIMARY KEY, walletid text NOT NULL UNIQUE )") ) )
                    throw __LINE__;

                lRet = true;
            }catch(int iLine){
                qWarning() << "Failed to initialize database on line" << iLine;
            }

            _commitTransaction(lDB);
        }
    }

    return lRet;
}

bool DBInterfaceV1::addressExists(const QString iAddress)
{
    bool            lRet            = false;
    QSqlDatabase    lDB;

    if( ! iAddress.isEmpty() ) {
        if( _connectOrReconnectToDB(lDB) ) {
            if( _beginTransactionLockTable(lDB,QStringLiteral("addresses"),false) ) {
                QSqlQuery   lQuery(lDB);

                lQuery.prepare( QStringLiteral("SELECT address FROM addresses WHERE address = :iAddress") );
                lQuery.bindValue(QStringLiteral(":iAddress"), _sanitizeAccountName(iAddress));

                if( lQuery.exec() ) {
                    int lAddressNo = lQuery.record().indexOf(QStringLiteral("address"));
                    if( lQuery.first() ) {
                        if( lQuery.value(lAddressNo).toString() == iAddress.toLower() ) {
                            lRet = true;
                        } else {
                            // It does not exist, however we can not use it because there is a record using the name partially?
                            qWarning() << "The database returned a record for user" << iAddress << ", however an internal string check returned that the username does not match what were looking for.";
                        }
                    }
                }

                _commitTransactionUnlockTable(lDB,QStringLiteral("addresses"));
            }
        }
    }

    return lRet;
}

bool DBInterfaceV1::addressCreate(const QString iAddress, const QByteArray iPublicKey)
{
    bool            lRet            = false;
    QSqlDatabase    lDB;

    if( ! iAddress.isEmpty() && ! iPublicKey.isEmpty() ) {

        if( _connectOrReconnectToDB(lDB) ) {
            if( _beginTransactionLockTable(lDB,QStringLiteral("addresses"),true) ) {
                QSqlQuery   lInsertUserQuery(lDB);
                QSqlQuery   lCreateTableQuery = _createEscrowTableSqlQueryForAccount(iAddress,lDB);

                lInsertUserQuery.prepare( QStringLiteral("INSERT INTO addresses (address, key) VALUES (:iAddress,:iPublicKey)") );
                lInsertUserQuery.bindValue( QStringLiteral(":iAddress"), _sanitizeAccountName(iAddress) );
                lInsertUserQuery.bindValue( QStringLiteral(":iPublicKey"), iPublicKey );


                if( lInsertUserQuery.exec() && lCreateTableQuery.exec() ) {
                    lRet = true;
                    _commitTransactionUnlockTable(lDB, QStringLiteral("addresses") );
                }else{
                    _rollbackTransaction(lDB);
                }
            }
        }
    }

    return lRet;
}

bool DBInterfaceV1::addressValidate(const QString iAddress, const QByteArray iPublicKey)
{
    bool            lRet            = false;
    QSqlDatabase    lDB;

    if( ! iAddress.isEmpty() && ! iPublicKey.isEmpty() ) {
        if( _connectOrReconnectToDB(lDB) ) {
            QString lAddress = _sanitizeAccountName(iAddress);
            if( _beginTransactionLockTable(lDB, QStringLiteral("addresses"), false) ) {
                QSqlQuery   lQuery(lDB);
                lQuery.prepare( QStringLiteral("SELECT address, key FROM addresses WHERE address = :iAddress") );
                lQuery.bindValue(QStringLiteral(":iAddress"), lAddress);

                if( lQuery.exec() ) {
                    int         lAddressNo  = lQuery.record().indexOf(QStringLiteral("address"));
                    int         lKeyNo      = lQuery.record().indexOf(QStringLiteral("key"));

                    if( lQuery.first() ) {
                        if( lQuery.value(lAddressNo).toString() == lAddress && lQuery.value(lKeyNo).toByteArray() == iPublicKey ) {
                            lRet = true;
                        } else {
                            // It does not exist, however we can not use it because there is a record using the name partially?
                            qWarning() << "The database returned a record for user" << iAddress << "(" << lAddress << "), however an internal string check returned that the username does not match what were looking for.";
                        }
                    }
                }
                _commitTransactionUnlockTable(lDB, QStringLiteral("addresses") );
            }
        }
    }

    return lRet;
}

bool DBInterfaceV1::addressDelete(const QString iAddress)
{
    bool            lRet            = false;
    QSqlDatabase    lDB;

    if( ! iAddress.isEmpty() ) {
        if( _connectOrReconnectToDB(lDB) ) {
            if( _beginTransactionLockTable(lDB, QStringLiteral("addresses"), true) ) {
                QSqlQuery   lDeleteUserQuery(lDB);
                QSqlQuery   lDropTableQuery(QStringLiteral("DROP TABLE %1").arg(_escrowTableNameForAccount(iAddress)), lDB);

                lDeleteUserQuery.prepare( QStringLiteral("DELETE FROM addresses WHERE address = :iAddress") );
                lDeleteUserQuery.bindValue( QStringLiteral(":iAddress"), _sanitizeAccountName(iAddress) );

                if( lDeleteUserQuery.exec() && lDropTableQuery.exec() ) {
                    _commitTransactionUnlockTable(lDB, QStringLiteral("addresses"));
                    lRet = true;
                }else{
                    _rollbackTransaction(lDB);
                }
            }
        }
    }

    return lRet;
}

QByteArray DBInterfaceV1::publicKeyForAddress(const QString iAddress)
{
    QByteArray      lRet;
    QSqlDatabase    lDB;

    if( ! iAddress.isEmpty() ) {
        if( _connectOrReconnectToDB(lDB) ) {
            if( _beginTransactionLockTable(lDB, QStringLiteral("addresses"), false) ) {
                QSqlQuery   lQuery(lDB);
                lQuery.prepare( QStringLiteral("SELECT key FROM addresses WHERE address = :iAddress") );
                lQuery.bindValue(QStringLiteral(":iAddress"), _sanitizeAccountName(iAddress));

                if( lQuery.exec() ) {
                    int         lKeyNo      = lQuery.record().indexOf(QStringLiteral("key"));

                    if( lQuery.first() ) {
                        lRet = lQuery.value(lKeyNo).toByteArray();
                    }
                }
                _commitTransactionUnlockTable(lDB, QStringLiteral("addresses"));
            }
        }
    }

    return lRet;
}

bool DBInterfaceV1::microWalletExists(const QString iMicroWalletId)
{
    bool            lRet            = false;
    QSqlDatabase    lDB;

    if( ! iMicroWalletId.isEmpty() ) {
        if( _connectOrReconnectToDB(lDB) ) {
            if( _beginTransactionLockTable(lDB, QStringLiteral("in_use_walletids"), false) ) {
                QSqlQuery   lQuery(lDB);
                QString     lMicroWalletId    = iMicroWalletId.toLower();
                lQuery.prepare( QStringLiteral("SELECT walletid FROM in_use_walletids WHERE walletid = :iMicroWalletId") );
                lQuery.bindValue( QStringLiteral(":iMicroWalletId"), lMicroWalletId );

                if( lQuery.exec() ) {
                    int         lWalletIdNo  = lQuery.record().indexOf(QStringLiteral("walletid"));
                    if( lQuery.first() ) {
                        if( lQuery.value(lWalletIdNo).toString() == lMicroWalletId ) {
                            lRet = true;
                        } else {
                            // It does not exist, however we can not use it because there is a record using the name partially?
                            qWarning() << "The database returned a record for wallet id" << iMicroWalletId << ", howere an internal string check returned that the wallet id does not match what were looking for.";
                        }
                    }
                }
                _commitTransactionUnlockTable(lDB, QStringLiteral("in_use_walletids"));
            }
        }
    }

    return lRet;
}

bool DBInterfaceV1::microWalletOwnershipCheck(const QString iMicroWalletId, const QString iAddress)
{
    bool            lRet            = false;
    QSqlDatabase    lDB;

    if( ! iMicroWalletId.isEmpty() && ! iAddress.isEmpty() ) {
        if( _connectOrReconnectToDB(lDB) ) {
            QString lTableName = _escrowTableNameForAccount(iAddress);
            if( _beginTransactionLockTable(lDB, lTableName, false) ) {
                QSqlQuery   lQuery(lDB);
                QString     lMicroWalletId    = iMicroWalletId.toLower();
                lQuery.prepare( QStringLiteral("SELECT walletid FROM %1 WHERE walletid = :iMicroWalletId AND owner = :iAddress").arg(lTableName) );
                lQuery.bindValue( QStringLiteral(":iMicroWalletId"), lMicroWalletId );

                if( lQuery.exec() ) {
                    int         lWalletIdNo  = lQuery.record().indexOf(QStringLiteral("walletid"));
                    if( lQuery.first() ) {
                        if( lQuery.value(lWalletIdNo).toString() == lMicroWalletId ) {
                            lRet = true;
                        } else {
                            // It does not exist, however we can not use it because there is a record using the name partially?
                            qWarning() << "The database returned a record for wallet id" << iMicroWalletId << ", howere an internal string check returned that the wallet id does not match what were looking for.";
                        }
                    }
                }

                _commitTransactionUnlockTable(lDB, lTableName);
            }
        }
    }

    return lRet;
}

bool DBInterfaceV1::microWalletChangeOwnership(const QString iMicroWalletId, const QString iFromAddress, const QString iToAddress)
{
    bool            lRet            = false;
    QSqlDatabase    lDB;

    if( ! iMicroWalletId.isEmpty() && ! iFromAddress.isEmpty() && ! iToAddress.isEmpty() ) {
        if( _connectOrReconnectToDB(lDB) ) {
            QString     lFromTable                      = _escrowTableNameForAccount(iFromAddress);
            QString     lToTable                        = _escrowTableNameForAccount(iToAddress);
            QString     lFromAddress                    = _sanitizeAccountName(iFromAddress);
            QString     lToAddress                      = _sanitizeAccountName(iToAddress);
            QString     lWalletId                       = iMicroWalletId.toLower();
            QStringList lTables                         = QStringList() << lFromTable << lToTable;
            QByteArray  lPayload;
            bool        lDoStep2                        = false;
            bool        lFixAfterStep1ButBeforeStep2    = false;

            /* Strategy:
             * 1. Change the owner in the from table, this will prevent the FROM from checking the wallet out, do this only if the FROM owner still matches.
             * 2. Copy the payload from the FROM.
             * 3. Commit the transaction.
             * 4. Begin new transaction.
             * 5. Insert the payload into the TO with the owner set to TO.
             * 6. Delete the record with the payload from the FROM.
             * 7. Commit the transaction.
             */

            if( _beginTransactionLockTable(lDB, lFromTable, true) ) {
                QSqlQuery       lChangeOwnerInFROMQuery(lDB);
                QSqlQuery       lCopyThePayloadInFROMQuery(lDB);

                lChangeOwnerInFROMQuery.prepare( QStringLiteral("UPDATE %1 SET owner = :iToAddress WHERE walletid = :iMicroWalletId AND owner = :iFromAddress").arg(lFromTable) );
                lChangeOwnerInFROMQuery.bindValue( QStringLiteral(":iToAddress"), lToAddress );
                lChangeOwnerInFROMQuery.bindValue( QStringLiteral(":iFromAddress"), lFromAddress );
                lChangeOwnerInFROMQuery.bindValue( QStringLiteral(":iMicroWalletId"), lWalletId );

                lCopyThePayloadInFROMQuery.prepare( QStringLiteral("SELECT payload FROM %1 WHERE walletid = :iMicroWalletId").arg(lFromTable) );
                lCopyThePayloadInFROMQuery.bindValue( QStringLiteral(":iMicroWalletId"), lWalletId );

                if( lChangeOwnerInFROMQuery.exec() && lCopyThePayloadInFROMQuery.exec() ) {
                    if( lChangeOwnerInFROMQuery.first() ) {
                        lPayload = lChangeOwnerInFROMQuery.value(lChangeOwnerInFROMQuery.record().indexOf(QStringLiteral("payload"))).toByteArray();
                        if( ! lPayload.isEmpty() ) {
                            lDoStep2 = true;
                        }
                    }
                }

                if( lDoStep2 )
                    _commitTransactionUnlockTable(lDB, lFromTable);
                else
                    _rollbackTransaction(lDB);
            }

            if( lDoStep2 && _beginTransactionLockTables(lDB, lTables, true) ) {
                QSqlQuery       lInsertThePayloadIntoTOQuery(lDB);
                QSqlQuery       lDeleteThePayloadFromFROMQuery(lDB);

                lInsertThePayloadIntoTOQuery.prepare( QStringLiteral("INSERT INTO %1 (walletid,owner,payload) VALUES(:iMicroWalletId,:iToAddress,:lPayload)").arg(lToTable) );
                lInsertThePayloadIntoTOQuery.bindValue( QStringLiteral(":iMicroWalletId"), lWalletId );
                lInsertThePayloadIntoTOQuery.bindValue( QStringLiteral(":iToAddress"), lToAddress );
                lInsertThePayloadIntoTOQuery.bindValue( QStringLiteral(":lPayload"), lPayload );

                lDeleteThePayloadFromFROMQuery.prepare( QStringLiteral("DELETE FROM %1 WHERE walletid = :iMicroWalletId").arg(lFromTable) );
                lDeleteThePayloadFromFROMQuery.bindValue( QStringLiteral(":iMicroWalletId"), lWalletId );

                if( lInsertThePayloadIntoTOQuery.exec() && lDeleteThePayloadFromFROMQuery.exec() ) {
                    lRet = true;
                }else{
                    lFixAfterStep1ButBeforeStep2 = true;
                }

                if( lRet )
                    _commitTransactionUnlockTables(lDB, lTables);
                else
                    _rollbackTransaction(lDB);

                if( lFixAfterStep1ButBeforeStep2 ) {
                    // If we are here then Step1 completed but Step2 did not, we need to roll back Step1
                    if( _beginTransactionLockTable(lDB, lFromTable, true) ) {
                        QSqlQuery lFixFROMQuery(lDB);
                        lFixFROMQuery.prepare( QStringLiteral("UPDATE %1 SET owner = :iFromAddress WHERE walletid = :iMicroWalletId AND owner = :iToAddress").arg(lFromTable) );
                        lFixFROMQuery.bindValue( QStringLiteral(":iToAddress"), lToAddress );
                        lFixFROMQuery.bindValue( QStringLiteral(":iFromAddress"), lFromAddress );
                        lFixFROMQuery.bindValue( QStringLiteral(":iMicroWalletId"), lWalletId );
                        if( lFixFROMQuery.exec() ) {
                            _commitTransactionUnlockTable(lDB, lFromTable);
                        }else{
                            qCritical() << "Unable to fix change ownership for wallet id" << lWalletId << "for transfer from" << iFromAddress << "(" << lFromAddress << ")" << "in table" << lFromTable << "to" << iToAddress << "(" << lToAddress << ")" << "to table" << lToTable << ", the wallet is most likely unusable until manually corrected.";
                            _rollbackTransaction(lDB);
                        }
                    }
                }
            }
        }
    }

    return lRet;
}
