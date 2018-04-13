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
    bool            lRet            = true;

    if( _connectOrReconnectToDB(lDB) ) {
        lDB.transaction();

        try {
            QSqlQuery   lQuery(lDB);
            // This might be the correct format for PSQL but does not work on SQLITE, keep for the future:
            // "CREATE TABLE IF NOT EXISTS addresses( rid integer PRIMARY KEY, address text NOT NULL, key blob, UNIQUE(rid, address) )"
            if( lQuery.exec( QStringLiteral("CREATE TABLE IF NOT EXISTS addresses( rid integer PRIMARY KEY, address text NOT NULL UNIQUE, key blob )") ) )
                lQuery.finish();
            else
                throw __LINE__;

            if( lQuery.exec( QStringLiteral("CREATE TABLE IF NOT EXISTS escrow( rid integer PRIMARY KEY, walletid text NOT NULL UNIQUE, owner text NOT NULL, payload blob )") ) )
                lQuery.finish();
            else
                throw __LINE__;

        }catch(int iLine){
            qWarning() << "Failed to initialize database on line" << iLine;
            lRet = false;
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

QByteArray DBInterfaceAlpha::publicKeyForAddress(const QString iAddress)
{
    QByteArray      lRet;
    QSqlDatabase    lDB;

    if( ! iAddress.isEmpty() ) {
        if( _connectOrReconnectToDB(lDB) ) {
            lDB.transaction();
            QSqlQuery   lQuery(lDB);
            QVariant    lAddress    = QVariant{QString{iAddress.toLower()}};
            lQuery.prepare( QStringLiteral("SELECT key FROM addresses WHERE address = :iAddress") );
            lQuery.bindValue(QStringLiteral(":iAddress"), lAddress);

            if( lQuery.exec() ) {
                int         lKeyNo      = lQuery.record().indexOf(QStringLiteral("key"));

                if( lQuery.first() ) {
                    lRet = lQuery.value(lKeyNo).toByteArray();
                }
            }

            _flushDB(lDB);
        }
    }

    return lRet;
}

bool DBInterfaceAlpha::microWalletExists(const QString iMicroWalletId)
{
    bool            lRet            = false;
    QSqlDatabase    lDB;

    if( ! iMicroWalletId.isEmpty() ) {

        if( _connectOrReconnectToDB(lDB) ) {
            lDB.transaction();
            QSqlQuery   lQuery(lDB);
            QVariant    lMicroWalletId    = QVariant{QString{iMicroWalletId.toLower()}};
            lQuery.prepare( QStringLiteral("SELECT walletid FROM escrow WHERE walletid = :iMicroWalletId") );
            lQuery.bindValue(QStringLiteral(":iMicroWalletId"), lMicroWalletId);

            int         lWalletIdNo  = lQuery.record().indexOf(QStringLiteral("walletid"));

            if( lQuery.exec() ) {
                if( lQuery.first() ) {
                    if( lQuery.value(lWalletIdNo).toString() == iMicroWalletId.toLower() ) {
                        lRet = true;
                    } else {
                        // It does not exist, however we can not use it because there is a record using the name partially?
                        qWarning() << "The database returned a record for wallet id" << iMicroWalletId << ", howere an internal string check returned that the wallet id does not match what were looking for.";
                    }
                }
            }

            _flushDB(lDB);
        }
    }

    return lRet;
}

bool DBInterfaceAlpha::microWalletOwnershipCheck(const QString iMicroWalletId, const QString iAddress)
{
    bool            lRet            = false;
    QSqlDatabase    lDB;

    if( ! iMicroWalletId.isEmpty() && ! iAddress.isEmpty() && _connectOrReconnectToDB(lDB) ) {
        QVariant    lAddress    =   QVariant{iAddress.toLower()};
        QVariant    lWalletId   =   QVariant{iMicroWalletId.toLower()};

        lDB.transaction();

        QSqlQuery   lQuery(lDB);
        lQuery.prepare( QStringLiteral("SELECT walletid FROM escrow WHERE walletid = :iMicroWalletId AND owner = :iAddress") );
        lQuery.bindValue(QStringLiteral(":iMicroWalletId"), lWalletId);
        lQuery.bindValue(QStringLiteral(":iAddress"), lAddress);

        int         lWalletIdNo  = lQuery.record().indexOf(QStringLiteral("walletid"));

        if( lQuery.exec() ) {
            if( lQuery.first() ) {
                if( lQuery.value(lWalletIdNo).toString() == iMicroWalletId.toLower() ) {
                    lRet = true;
                } else {
                    // It does not exist, however we can not use it because there is a record using the name partially?
                    qWarning() << "The database returned a record for wallet id" << iMicroWalletId << ", howere an internal string check returned that the wallet id does not match what were looking for.";
                }
            }
        }

        _flushDB(lDB);
    }

    return lRet;
}

bool DBInterfaceAlpha::microWalletChangeOwnership(const QString iMicroWalletId, const QString iFromAddress, const QString iToAddress)
{
    bool            lRet            = false;
    QSqlDatabase    lDB;

    if( ! iMicroWalletId.isEmpty() && ! iFromAddress.isEmpty() && ! iToAddress.isEmpty() && _connectOrReconnectToDB(lDB) ) {
        QVariant    lFromAddress    =   QVariant{iFromAddress.toLower()};
        QVariant    lToAddress      =   QVariant{iToAddress.toLower()};
        QVariant    lWalletId       =   QVariant{iMicroWalletId.toLower()};

        lDB.transaction();

        QSqlQuery   lQuery(lDB);
        lQuery.prepare( QStringLiteral("UPDATE escrow SET owner = :iToAddress WHERE walletid = :iMicroWalletId AND owner = :iFromAddress") );
        lQuery.bindValue(QStringLiteral(":iMicroWalletId"), lWalletId);
        lQuery.bindValue(QStringLiteral(":iToAddress"), lToAddress);
        lQuery.bindValue(QStringLiteral(":iFromAddress"), lFromAddress);


        if( lQuery.exec() ) {
            lQuery.finish();
            lRet = true;
        }

        _flushDB(lDB);
    }

    return lRet;
}

bool DBInterfaceAlpha::microWalletCreate(const QString iMicroWalletId, const QString iAddress, const QByteArray iPayload)
{
    bool            lRet            = false;
    QSqlDatabase    lDB;

    if( ! iMicroWalletId.isEmpty() && ! iAddress.isEmpty() && ! iPayload.isEmpty() && _connectOrReconnectToDB(lDB) ) {
        QVariant    lPayload        =   QVariant{iPayload.toBase64()};
        QVariant    lAddress        =   QVariant{iAddress.toLower()};
        QVariant    lWalletId       =   QVariant{iMicroWalletId.toLower()};

        lDB.transaction();

        QSqlQuery   lQuery(lDB);
        lQuery.prepare( QStringLiteral("INSERT INTO escrow (walletid,owner,payload) VALUES(:iMicroWalletId,:iAddress,:iPayload)") );
        lQuery.bindValue(QStringLiteral(":iMicroWalletId"), lWalletId);
        lQuery.bindValue(QStringLiteral(":iAddress"), lAddress);
        lQuery.bindValue(QStringLiteral(":iPayload"), lPayload);


        if( lQuery.exec() ) {
            lQuery.finish();
            lRet = true;
        }

        _flushDB(lDB);
    }

    return lRet;
}
