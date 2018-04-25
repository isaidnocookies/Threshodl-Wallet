#include "dbinterfacev1.h"

#include <QDebug>
#include <QStringList>
#include <QRegExp>
#include <QVariant>
#include <QSqlDatabase>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QSqlError>

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
    oDatabase.setPort(mPort);
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
            QSqlQuery(iDatabase).exec(QStringLiteral( "LOCK TABLE %1 IN %2 MODE" )
                           .arg(iTableName)
                           .arg( iLockExclusive ? QStringLiteral("EXCLUSIVE") : QStringLiteral("SHARE") )
                                )
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
            if( ! QSqlQuery(iDatabase).exec(
                        QStringLiteral( "LOCK TABLE %1 IN %2 MODE" )
                        .arg( lTN )
                        .arg( iLockExclusive ? QStringLiteral("EXCLUSIVE") : QStringLiteral("SHARE") )
                        )
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

QString DBInterfaceV1::_createEscrowTableSqlQueryForAccount(const QString iAccountName)
{
    return QStringLiteral("CREATE TABLE %1 ( rid SERIAL PRIMARY KEY, walletid text NOT NULL UNIQUE, state integer, payload text );")
                .arg(_escrowTableNameForAccount(iAccountName));
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
                if( ! lQuery.exec( QStringLiteral("CREATE TABLE IF NOT EXISTS addresses( rid SERIAL PRIMARY KEY, address text NOT NULL UNIQUE, key text )") ) )
                    throw __LINE__;

                if( ! lQuery.exec( QStringLiteral("CREATE TABLE IF NOT EXISTS in_use_walletids( rid SERIAL PRIMARY KEY, walletid text NOT NULL UNIQUE )") ) )
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
                QSqlQuery   lCreateTableQuery(lDB);

                lInsertUserQuery.prepare( QStringLiteral("INSERT INTO addresses (address, key) VALUES (:iAddress,:iPublicKey);") );
                lInsertUserQuery.bindValue( QStringLiteral(":iAddress"), _sanitizeAccountName(iAddress) );
                lInsertUserQuery.bindValue( QStringLiteral(":iPublicKey"), QString::fromUtf8(iPublicKey) );


                if( lInsertUserQuery.exec() && lCreateTableQuery.exec(_createEscrowTableSqlQueryForAccount(iAddress)) ) {
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
                        if( lQuery.value(lAddressNo).toString() == lAddress && lQuery.value(lKeyNo) == QString::fromUtf8(iPublicKey) ) {
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
                        lRet = lQuery.value(lKeyNo).toString().toUtf8();
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
    return microWalletExists(QStringList() << iMicroWalletId);
//    bool            lRet            = false;
//    QSqlDatabase    lDB;

//    if( ! iMicroWalletId.isEmpty() ) {
//        if( _connectOrReconnectToDB(lDB) ) {
//            if( _beginTransactionLockTable(lDB, QStringLiteral("in_use_walletids"), false) ) {
//                QSqlQuery   lQuery(lDB);
//                QString     lMicroWalletId    = iMicroWalletId.toLower();
//                lQuery.prepare( QStringLiteral("SELECT walletid FROM in_use_walletids WHERE walletid = :iMicroWalletId") );
//                lQuery.bindValue( QStringLiteral(":iMicroWalletId"), lMicroWalletId );

//                if( lQuery.exec() ) {
//                    int         lWalletIdNo  = lQuery.record().indexOf(QStringLiteral("walletid"));
//                    if( lQuery.first() ) {
//                        if( lQuery.value(lWalletIdNo).toString() == lMicroWalletId ) {
//                            lRet = true;
//                        } else {
//                            // It does not exist, however we can not use it because there is a record using the name partially?
//                            qWarning() << "The database returned a record for wallet id" << iMicroWalletId << ", howere an internal string check returned that the wallet id does not match what were looking for.";
//                        }
//                    }
//                }
//                _commitTransactionUnlockTable(lDB, QStringLiteral("in_use_walletids"));
//            }
//        }
//    }

//    return lRet;
}

bool DBInterfaceV1::microWalletExists(const QStringList iMicroWalletId)
{
    bool            lRet            = false;
    QSqlDatabase    lDB;

    if( ! iMicroWalletId.isEmpty() ) {
        if( _connectOrReconnectToDB(lDB) ) {
            if( _beginTransactionLockTable(lDB, QStringLiteral("in_use_walletids"), false) ) {
                int lFoundCount = 0;
                for( QString lEntry : iMicroWalletId ) {
                    QSqlQuery   lQuery(lDB);
                    QString     lMicroWalletId    = lEntry.toLower();

                    if( lQuery.exec(
                        QStringLiteral("SELECT walletid FROM in_use_walletids WHERE walletid = '%1'")
                        .arg(lMicroWalletId)
                            )
                    ) {
                        int         lWalletIdNo  = lQuery.record().indexOf(QStringLiteral("walletid"));
                        if( lQuery.first() ) {
                            if( lQuery.value(lWalletIdNo).toString() == lMicroWalletId ) {
                                lQuery.finish();
                                lFoundCount++;
                            } else {
                                // It does not exist, however we can not use it because there is a record using the name partially?
                                qWarning() << "The database returned a record for wallet id" << iMicroWalletId << ", howere an internal string check returned that the wallet id does not match what were looking for.";
                            }
                        }
                    }
                }

                if( lFoundCount == iMicroWalletId.count() )
                    lRet = true;

                _commitTransactionUnlockTable(lDB, QStringLiteral("in_use_walletids"));
            }
        }
    }

    return lRet;
}

bool DBInterfaceV1::microWalletOwnershipCheck(const QString iMicroWalletId, const QString iAddress)
{
    return microWalletOwnershipCheck(QStringList() << iMicroWalletId, iAddress);
//    bool            lRet            = false;
//    QSqlDatabase    lDB;

//    if( ! iMicroWalletId.isEmpty() && ! iAddress.isEmpty() ) {
//        if( _connectOrReconnectToDB(lDB) ) {
//            QString lTableName = _escrowTableNameForAccount(iAddress);
//            if( _beginTransactionLockTable(lDB, lTableName, false) ) {
//                QSqlQuery   lQuery(lDB);
//                QString     lMicroWalletId    = iMicroWalletId.toLower();
//                lQuery.prepare( QStringLiteral("SELECT walletid FROM %1 WHERE walletid = :iMicroWalletId AND state = %2").arg(lTableName).arg(static_cast<int>(EscrowRecordState::Unlocked)));
//                lQuery.bindValue( QStringLiteral(":iMicroWalletId"), lMicroWalletId );

//                if( lQuery.exec() ) {
//                    int         lWalletIdNo  = lQuery.record().indexOf(QStringLiteral("walletid"));
//                    if( lQuery.first() ) {
//                        if( lQuery.value(lWalletIdNo).toString() == lMicroWalletId ) {
//                            lRet = true;
//                        } else {
//                            // It does not exist, however we can not use it because there is a record using the name partially?
//                            qWarning() << "The database returned a record for wallet id" << iMicroWalletId << ", howere an internal string check returned that the wallet id does not match what were looking for.";
//                        }
//                    }
//                } else {
//                    qWarning() << lQuery.executedQuery() << lQuery.lastError();
//                }

//                _commitTransactionUnlockTable(lDB, lTableName);
//            }
//        }
//    }

//    return lRet;
}

bool DBInterfaceV1::microWalletOwnershipCheck(const QStringList iMicroWalletIds, const QString iAddress)
{
    bool            lRet            = false;
    QSqlDatabase    lDB;

    if( ! iMicroWalletIds.isEmpty() && ! iAddress.isEmpty() ) {
        if( _connectOrReconnectToDB(lDB) ) {
            QString lTableName = _escrowTableNameForAccount(iAddress);
            if( _beginTransactionLockTable(lDB, lTableName, false) ) {
                int lFoundCount = 0;
                for( QString lEntry : iMicroWalletIds )
                {
                    QSqlQuery   lQuery(lDB);
                    QString     lMicroWalletId    = lEntry.toLower();

                    if( lQuery.exec(
                            QStringLiteral("SELECT walletid FROM %1 WHERE walletid = '%3' AND state = %2").arg(lTableName).arg(static_cast<int>(EscrowRecordState::Unlocked)).arg(lMicroWalletId)
                    ) ) {
                        int         lWalletIdNo  = lQuery.record().indexOf(QStringLiteral("walletid"));
                        if( lQuery.first() ) {
                            if( lQuery.value(lWalletIdNo).toString() == lMicroWalletId ) {
                                lQuery.finish();
                                lFoundCount++;
                            } else {
                                // It does not exist, however we can not use it because there is a record using the name partially?
                                qWarning() << "The database returned a record for wallet id" << iMicroWalletIds << ", howere an internal string check returned that the wallet id does not match what were looking for.";
                            }
                        }
                    } else {
                        qWarning() << lQuery.executedQuery() << lQuery.lastError();
                    }
                }

                if( lFoundCount == iMicroWalletIds.count() )
                    lRet = true;

                _commitTransactionUnlockTable(lDB, lTableName);
            }
        }
    }

    return lRet;
}

bool DBInterfaceV1::microWalletChangeOwnership(const QString iMicroWalletId, const QString iFromAddress, const QString iToAddress)
{ return microWalletChangeOwnership(QStringList() << iMicroWalletId, iFromAddress, iToAddress); }

bool DBInterfaceV1::microWalletChangeOwnership(const QStringList iMicroWalletIds, const QString iFromAddress, const QString iToAddress)
{
    if( iFromAddress == iToAddress ) return true;

    bool            lRet        = false;
    QSqlDatabase    lDB;

    if( ! iMicroWalletIds.isEmpty() && ! iFromAddress.isEmpty() && ! iToAddress.isEmpty() ) {
        if( _connectOrReconnectToDB(lDB) ) {
            QString     lFromTable                      = _escrowTableNameForAccount(iFromAddress);
            QString     lToTable                        = _escrowTableNameForAccount(iToAddress);
            QStringList lTables                         = QStringList() << lFromTable << lToTable;
            int         lCompletedWalletsCount          = 0;

            if( _beginTransactionLockTables(lDB, lTables, true) ) {
                for( QString lEntry : iMicroWalletIds ) {
                    QString     lWalletId                   = lEntry.toLower();

                    QSqlQuery   lUpdateStateInFROM(lDB);
                    QSqlQuery   lCopyRecordInTO(lDB);
                    QSqlQuery   lDeleteRecordInFROM(lDB);
                    QSqlQuery   lUpdateStateInTO(lDB);

                    if( lUpdateStateInFROM.exec(
                                QStringLiteral("UPDATE %1 SET state = %2 WHERE walletid = '%4' AND state = %3")
                                .arg(lFromTable)
                                .arg(static_cast<int>(EscrowRecordState::Locked))
                                .arg(static_cast<int>(EscrowRecordState::Unlocked))
                                .arg(lWalletId)
                                ) && lUpdateStateInFROM.numRowsAffected() == 1 ) {
                        // Now execute the copy
                        if( lCopyRecordInTO.exec(QStringLiteral("INSERT INTO %1 (walletid, state, payload) SELECT walletid, state, payload FROM %2 WHERE state = %3 AND walletid = '%4'")
                                                 .arg(lToTable)
                                                 .arg(lFromTable)
                                                 .arg(static_cast<int>(EscrowRecordState::Locked))
                                                 .arg(lWalletId)
                                                 ) ) {
                            if( lDeleteRecordInFROM.exec(
                                        QStringLiteral("DELETE FROM %1 WHERE state = %2 AND walletid = '%3'")
                                        .arg(lFromTable)
                                        .arg(static_cast<int>(EscrowRecordState::Locked))
                                        .arg(lWalletId)
                                        ) ) {
                                if( lUpdateStateInTO.exec(
                                            QStringLiteral("UPDATE %1 SET state = %2 WHERE walletid = '%4' AND state = %3")
                                            .arg(lToTable)
                                            .arg(static_cast<int>(EscrowRecordState::Unlocked))
                                            .arg(static_cast<int>(EscrowRecordState::Locked))
                                            .arg(lWalletId)
                                            ) && lUpdateStateInTO.numRowsAffected() == 1 ) {
                                    // Finished!
                                    lCompletedWalletsCount++;
                                } else {
                                    // Update state on TO failed
                                    qWarning() << "Failed to update the micro wallet internal state on the new owner table. Will rollback. WalletId:" << lWalletId << "From:" << iFromAddress << "FromTable:" << lFromTable << "To:" << iToAddress << "ToTable:" << lToTable;
                                    break;
                                }
                            } else {
                                // Delete failed
                                qWarning() << "Failed to delete the micro wallet from the old owner table. Will rollback. WalletId:" << lWalletId << "From:" << iFromAddress << "FromTable:" << lFromTable << "To:" << iToAddress << "ToTable:" << lToTable;
                                break;
                            }
                        } else {
                            // Copy failed
                            qWarning() << "Failed to copy the micro wallet to the new owner table. Will rollback. WalletId:" << lWalletId << "From:" << iFromAddress << "FromTable:" << lFromTable << "To:" << iToAddress << "ToTable:" << lToTable;
                            qWarning() << lCopyRecordInTO.executedQuery() << lCopyRecordInTO.lastError();
                            break;
                        }
                    } else {
                        // Record not locked
                        qWarning() << "Unable to update the micro wallet internal state on the original owner table. Will rollback. WalletId:" << lWalletId << "From:" << iFromAddress << "FromTable:" << lFromTable << "To:" << iToAddress << "ToTable:" << lToTable;
                        qWarning() << lUpdateStateInFROM.executedQuery() << lUpdateStateInFROM.lastError();
                        break;
                    }

                    // end of for loop
                }

                if( lCompletedWalletsCount == iMicroWalletIds.count() ) {
                    // Success
                    lRet = true;
                }

                // Either commit or roll back the transactions
                if( lRet )
                    _commitTransactionUnlockTables(lDB,lTables);
                else
                    _rollbackTransaction(lDB);

            } else {
                // Couldn't start transaction
                qCritical() << "Could not start a new database transaction.";
            }
        } else {
            // Couldn't connect to DB
            qCritical() << "Could not connect to the database.";
        }
    } else {
        // Missing arg
        qWarning() << "One or more arguments is missing or invalid.";
    }

    return lRet;
}

bool DBInterfaceV1::microWalletCreate(const QString iMicroWalletId, const QString iAddress, const QByteArray iPayload)
{
    bool            lRet            = false;
    QSqlDatabase    lDB;

    if( ! iMicroWalletId.isEmpty() && ! iAddress.isEmpty() && ! iPayload.isEmpty() && _connectOrReconnectToDB(lDB) ) {
        QString     lWalletId       = iMicroWalletId.toLower();
        QString     lTableName      = _escrowTableNameForAccount(iAddress);
        QStringList lTableNames     = QStringList() << lTableName << QStringLiteral("in_use_walletids");

        if( _beginTransactionLockTables(lDB,lTableNames,true) ) {
            QSqlQuery   lInsertIntoEscrow(lDB);
            QSqlQuery   lInsertIntoInUse(lDB);

            lInsertIntoInUse.prepare(
                        QStringLiteral("INSERT INTO in_use_walletids (walletid) VALUES(:iMicroWalletId)")
                        );
            lInsertIntoInUse.bindValue(QStringLiteral(":iMicroWalletId"), lWalletId);

            lInsertIntoEscrow.prepare(
                        QStringLiteral("INSERT INTO %1 (walletid,state,payload) VALUES(:iMicroWalletId,%2,:iPayload)")
                        .arg(lTableName)
                        .arg(static_cast<int>(EscrowRecordState::Unlocked))
                        );
            lInsertIntoEscrow.bindValue(QStringLiteral(":iMicroWalletId"), lWalletId);
            lInsertIntoEscrow.bindValue(QStringLiteral(":iPayload"), QString::fromUtf8(iPayload.toBase64()) );

            if( lInsertIntoInUse.exec() && lInsertIntoEscrow.exec() && lInsertIntoInUse.numRowsAffected() == 1 && lInsertIntoEscrow.numRowsAffected() == 1 ) {
                lRet = true;
                _commitTransactionUnlockTables(lDB,lTableNames);
            }else{
                _rollbackTransaction(lDB);
            }
        }
    }

    return lRet;
}

QByteArray DBInterfaceV1::microWalletCopyPayload(const QString iMicroWalletId, const QString iAddress)
{
    QByteArray      lRet;
    QSqlDatabase    lDB;

    if( ! iMicroWalletId.isEmpty() && ! iAddress.isEmpty() && _connectOrReconnectToDB(lDB) ) {
        QString lWalletId   = iMicroWalletId.toLower();
        QString lTableName  = _escrowTableNameForAccount(iAddress);

        if( _beginTransactionLockTable(lDB,lTableName,false) ) {

            QSqlQuery   lQuery(lDB);
            lQuery.prepare(
                        QStringLiteral("SELECT payload FROM %1 WHERE walletid = :iMicroWalletId AND state = %2")
                        .arg(lTableName)
                        .arg(static_cast<int>(EscrowRecordState::Unlocked))
                        );
            lQuery.bindValue(QStringLiteral(":iMicroWalletId"), lWalletId);

            if( lQuery.exec() ) {
                int lPayloadNo = lQuery.record().indexOf(QStringLiteral("payload"));
                if( lQuery.first() ) {
                    lRet = QByteArray::fromBase64(lQuery.value(lPayloadNo).toString().toUtf8());
                }
            }
        }

        _commitTransactionUnlockTable(lDB,lTableName);
    }

    return lRet;
}

bool DBInterfaceV1::microWalletDelete(const QString iMicroWalletId, const QString iAddress)
{
    bool            lRet            = false;
    QSqlDatabase    lDB;

    if( ! iMicroWalletId.isEmpty() && ! iAddress.isEmpty() && _connectOrReconnectToDB(lDB) ) {
        QString lTableName  = _escrowTableNameForAccount(iAddress);

        if( _beginTransactionLockTable(lDB,lTableName) ) {
            QSqlQuery   lQuery(lDB);
            QString     lWalletId   = iMicroWalletId.toLower();

            lQuery.prepare(
                        QStringLiteral("DELETE FROM %1 WHERE walletid = :iMicroWalletId AND state = %2")
                        .arg(lTableName)
                        .arg(static_cast<int>(EscrowRecordState::Unlocked))
                        );
            lQuery.bindValue(QStringLiteral(":iMicroWalletId"), lWalletId);

            if( lQuery.exec() && lQuery.numRowsAffected() == 1 ) {
                lRet = true;
                _commitTransactionUnlockTable(lDB,lTableName);
            }else{
                _rollbackTransaction(lDB);
            }
        }
    }

    return lRet;
}
