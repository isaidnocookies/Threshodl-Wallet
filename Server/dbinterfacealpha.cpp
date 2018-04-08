#include "dbinterfacealpha.h"

#include <QDebug>
#include <QVariant>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>

bool DBInterfaceAlpha::_connected()
{
    QMutexLocker l{&mConnectionCounterLock};
    return (mConnectionCounter != 0);
}

bool DBInterfaceAlpha::_connectOrReconnectToDB(QSqlDatabase &oDatabase)
{
    if( _connected() ) {
        return _reconnectToDB(lastConnectionCounterString(),oDatabase);
    }

    QString lCounter;
    return _connectToDB(lCounter,oDatabase);
}

bool DBInterfaceAlpha::_connectToDB(QString &oConnectionCounterString, QSqlDatabase &oDatabase)
{
    oConnectionCounterString = _nextConnectionCounterString();
    oDatabase = QSqlDatabase::addDatabase(mSqlType,oConnectionCounterString);
    oDatabase.setHostName(mHostName);
    oDatabase.setDatabaseName(mDatabaseName);
    oDatabase.setUserName(mUserName);
    oDatabase.setPassword(mPassword);
    return oDatabase.open();
}

bool DBInterfaceAlpha::_reconnectToDB(const QString iConnectionCounterString, QSqlDatabase &oDatabase)
{
    oDatabase = QSqlDatabase::database(iConnectionCounterString,true);
    return oDatabase.isOpen();
}

bool DBInterfaceAlpha::_flushDB(QSqlDatabase &iDatabase)
{
    return iDatabase.commit();
}

void DBInterfaceAlpha::_closeDB(const QString iConnectionCounterString, QSqlDatabase &iDatabase)
{
    iDatabase.commit();
    iDatabase.close();
    QSqlDatabase::removeDatabase(iConnectionCounterString);
}

QString DBInterfaceAlpha::lastConnectionCounterString() const
{
    return mLastConnectionCounterString;
}

DBInterfaceAlpha::DBInterfaceAlpha(const QString iUserName, const QString iPassword, const QString iDatabaseName, const QString iHostName, quint16 iPort)
    : DBInterface()
    , mUserName(iUserName), mPassword(iPassword)
    , mDatabaseName(iDatabaseName), mHostName(iHostName)
    , mPort(iPort)
{

}

void DBInterfaceAlpha::setSqlType(const QString iSqlType)
{
    mSqlType = iSqlType;
}

QString DBInterfaceAlpha::sqlType() const
{
    return mSqlType;
}

bool DBInterfaceAlpha::initDB()
{
    QSqlDatabase    lDB;
    bool            lRet            = false;

    if( _connectOrReconnectToDB(lDB) ) {
        lDB.transaction();

        QSqlQuery   lQuery(lDB);

        // "CREATE TABLE IF NOT EXISTS addresses( rid integer PRIMARY KEY, address text NOT NULL, key blob, UNIQUE(rid, address) )"
        if( lQuery.exec( QStringLiteral("CREATE TABLE IF NOT EXISTS addresses( rid integer PRIMARY KEY, address text NOT NULL UNIQUE, key blob )") ) ) {
            lQuery.finish();
            lRet = true;
        }

        _flushDB(lDB);
    }

    return lRet;
}

bool DBInterfaceAlpha::addressExists(const QString iAddress)
{
    bool            lRet            = false;
    QSqlDatabase    lDB;

    if( ! iAddress.isEmpty() ) {

        if( _connectOrReconnectToDB(lDB) ) {
            lDB.transaction();
            QSqlQuery   lQuery(lDB);
            QVariant    lAddress    = QVariant{QString{iAddress.toLower()}};
            lQuery.prepare( QStringLiteral("SELECT address FROM addresses WHERE address = :iAddress") );
            lQuery.bindValue(QStringLiteral(":iAddress"), lAddress);

            int         lAddressNo  = lQuery.record().indexOf(QStringLiteral("address"));

            if( lQuery.exec() ) {
                if( lQuery.first() ) {
                    if( lQuery.value(lAddressNo).toString() == iAddress.toLower() ) {
                        lRet = true;
                    } else {
                        // It does not exist, however we can not use it because there is a record using the name partially?
                        qWarning() << "The database returned a record for user" << iAddress << ", howere an internal string check returned that the username does not match what were looking for.";
                    }
                }
            }

            _flushDB(lDB);
        }
    }

    return lRet;
}

bool DBInterfaceAlpha::addressCreate(const QString iAddress, const QByteArray iPublicKey)
{
    bool            lRet            = false;
    QSqlDatabase    lDB;

    if( ! iAddress.isEmpty() && ! iPublicKey.isEmpty() ) {

        if( _connectOrReconnectToDB(lDB) ) {
            lDB.transaction();

            QSqlQuery   lQuery(lDB);
            QVariant    lAddress    = QVariant{QString{iAddress.toLower()}};
            QVariant    lPublicKey  = QVariant{QByteArray{iPublicKey}};
            lQuery.prepare( QStringLiteral("INSERT INTO addresses (address, key) VALUES (:iAddress,:iPublicKey)") );
            lQuery.bindValue(QStringLiteral(":iAddress"), lAddress);
            lQuery.bindValue(QStringLiteral(":iPublicKey"),lPublicKey);

            if( lQuery.exec() ) {
                lQuery.finish();
                lRet = true;
            }

            _flushDB(lDB);
        }
    }

    return lRet;
}

bool DBInterfaceAlpha::addressValidate(const QString iAddress, const QByteArray iPublicKey)
{
    bool            lRet            = false;
    QSqlDatabase    lDB;

    if( ! iAddress.isEmpty() ) {
        if( _connectOrReconnectToDB(lDB) ) {
            lDB.transaction();
            QSqlQuery   lQuery(lDB);
            QVariant    lAddress    = QVariant{QString{iAddress.toLower()}};
            lQuery.prepare( QStringLiteral("SELECT address, key FROM addresses WHERE address = :iAddress") );
            lQuery.bindValue(QStringLiteral(":iAddress"), lAddress);

            if( lQuery.exec() ) {
                int         lAddressNo  = lQuery.record().indexOf(QStringLiteral("address"));
                int         lKeyNo      = lQuery.record().indexOf(QStringLiteral("key"));

                if( lQuery.first() ) {
                    if( lQuery.value(lAddressNo).toString() == iAddress.toLower() && lQuery.value(lKeyNo).toByteArray() == iPublicKey ) {
                        lRet = true;
                    } else {
                        // It does not exist, however we can not use it because there is a record using the name partially?
                        qWarning() << "The database returned a record for user" << iAddress << ", howere an internal string check returned that the username does not match what were looking for.";
                    }
                }
            }

            _flushDB(lDB);
        }
    }

    return lRet;
}

bool DBInterfaceAlpha::addressDelete(const QString iAddress)
{
    bool            lRet            = false;
    QSqlDatabase    lDB;

    if( ! iAddress.isEmpty() ) {

        if( _connectOrReconnectToDB(lDB) ) {
            lDB.transaction();
            QSqlQuery   lQuery(lDB);
            QVariant    lAddress    = QVariant{QString{iAddress.toLower()}};
            lQuery.prepare( QStringLiteral("DELETE FROM addresses WHERE address = :iAddress") );
            lQuery.bindValue(QStringLiteral(":iAddress"), lAddress);

            if( lQuery.exec() ) {
                lRet = true;
            }

            _flushDB(lDB);
        }
    }

    return lRet;
}
