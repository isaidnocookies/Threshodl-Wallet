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

bool DBInterfaceV1::_commitTransaction(QSqlDatabase &iDatabase)
{ return iDatabase.commit(); }

bool DBInterfaceV1::_commitTransactionUnlockTable(QSqlDatabase &iDatabase, const QString iTableName)
{ Q_UNUSED(iTableName) return _commitTransaction(iDatabase); }

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

QString DBInterfaceV1::_createEscrowTableSqlCommandForAccount(const QString iAccountName)
{ return QStringLiteral("CREATE TABLE %1( rid integer PRIMARY KEY, walletid text NOT NULL UNIQUE, owner text NOT NULL, payload blob )").arg(_escrowTableNameForAccount(iAccountName)); }

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
                QSqlQuery   lInsertQuery(lDB);

                lInsertQuery.prepare( QStringLiteral("INSERT INTO addresses (address, key) VALUES (:iAddress,:iPublicKey)") );
                lInsertQuery.bindValue(QStringLiteral(":iAddress"), _sanitizeAccountName(iAddress));
                lInsertQuery.bindValue(QStringLiteral(":iPublicKey"), iPublicKey);

                if( lInsertQuery.exec() && QSqlQuery(lDB).exec(_createEscrowTableSqlCommandForAccount(iAddress)) ) {
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
#warning NEED to delete associated escrow table(s)
    bool            lRet            = false;
    QSqlDatabase    lDB;

    if( ! iAddress.isEmpty() ) {
        if( _connectOrReconnectToDB(lDB) ) {
            if( _beginTransactionLockTable(lDB, QStringLiteral("addresses"), true) ) {
                QSqlQuery   lQuery(lDB);
                lQuery.prepare( QStringLiteral("DELETE FROM addresses WHERE address = :iAddress") );
                lQuery.bindValue(QStringLiteral(":iAddress"), _sanitizeAccountName(iAddress));

                if( lQuery.exec() ) {
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
            }
            _commitTransactionUnlockTable(lDB, QStringLiteral("addresses"));
        }
    }

    return lRet;
}
