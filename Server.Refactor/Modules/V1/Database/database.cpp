#include "database.h"
#include "modulelinker.h"
#include "app.h"
#include "databasepriv.h"
#include "databaseprivpsql.h"

static DatabaseML _gRegisterModuleLinker;

DatabaseML::DatabaseML()
{
    ModuleLinker::registerModule(QStringLiteral("Database-1"),DatabaseML::creator,DatabaseML::doInit,DatabaseML::start,DatabaseML::startInOwnThread);
}

void *DatabaseML::creator(void *pointerToAppObject)
{
    App *       lApp    = reinterpret_cast<App *>(pointerToAppObject);
    if( lApp ) {
        auto    lConfig = lApp->configuration();
        QString lDBType = QStringLiteral("QPSQL");

        if( lConfig->contains(QStringLiteral("DatabaseType")) )
            lDBType = lConfig->toString(QStringLiteral("DatabaseType"));
        else if( lConfig->contains(QStringLiteral("DBType")) )
            lDBType = lConfig->toString(QStringLiteral("DBType"));

        Database *  lDB     = new Database{lDBType};
        lDB->mApp = lApp;
        return lDB;
    }

    return nullptr;
}

bool DatabaseML::doInit(void *pointerToThis, void *pointerToAppObject)
{
    Q_UNUSED(pointerToAppObject)
    Database *  lDB = reinterpret_cast<Database *>(pointerToThis);
    if( lDB )
        return lDB->createTables();
    return false;
}

bool DatabaseML::startInOwnThread()
{ return false; }

bool DatabaseML::start(void *pointerToThis, void *pointerToAppObject)
{
    Q_UNUSED(pointerToAppObject)
    Database *  lDB = reinterpret_cast<Database *>(pointerToThis);
    if( lDB ) {
        return lDB->_createPriv();
    }
    return false;
}

bool Database::_createPriv()
{
    QString lType = databaseType().toLower();

    if( lType == QStringLiteral("psql") || lType == QStringLiteral("qpsql") ) {
        DatabasePrivPSQL *  lPriv   = new DatabasePrivPSQL;
        lPriv->mApp                 = mApp;
        mPriv                       = dynamic_cast<DatabasePriv *>(lPriv);
        return lPriv->_init();
    }

    return false;
}

Database::Database(const QString iDatabaseType) : DatabaseInterface(iDatabaseType)
{ }

Database::~Database()
{
    if( mPriv )
        delete mPriv;
}

bool Database::createTables()
{
    if( ! mPriv ) {
        if( ! _createPriv() )
            return false;
    }

    return mPriv->createTables();
}

bool Database::addressExists(const QString iAddress)
{ return mPriv->addressExists(iAddress); }

bool Database::addressCreate(const QString iAddress, const QByteArray iPublicKey)
{ return mPriv->addressCreate(iAddress,iPublicKey); }

bool Database::addressValidate(const QString iAddress, const QByteArray iPublicKey)
{ return mPriv->addressValidate(iAddress,iPublicKey); }

bool Database::addressDelete(const QString iAddress)
{ return mPriv->addressDelete(iAddress); }

QByteArray Database::publicKeyForAddress(const QString iAddress)
{ return mPriv->publicKeyForAddress(iAddress); }

bool Database::microWalletExists(const QString iMicroWalletId)
{ return mPriv->microWalletsExists(QStringList() << iMicroWalletId); }

bool Database::microWalletsExists(const QStringList iMicroWalletIds)
{ return mPriv->microWalletsExists(iMicroWalletIds); }

bool Database::microWalletOwnershipCheck(const QString iMicroWalletId, const QString iAddress)
{ return mPriv->microWalletsOwnershipCheck(QStringList() << iMicroWalletId, iAddress); }

bool Database::microWalletsOwnershipCheck(const QStringList iMicroWalletIds, const QString iAddress)
{ return mPriv->microWalletsOwnershipCheck(iMicroWalletIds,iAddress); }

bool Database::microWalletChangeOwnership(const QString iMicroWalletId, const QString iFromAddress, const QString iToAddress)
{ return mPriv->microWalletsChangeOwnership(QStringList() << iMicroWalletId, iFromAddress, iToAddress); }

bool Database::microWalletsChangeOwnership(const QStringList iMicroWalletIds, const QString iFromAddress, const QString iToAddress)
{ return mPriv->microWalletsChangeOwnership(iMicroWalletIds, iFromAddress, iToAddress); }

bool Database::microWalletCreate(const QString iMicroWalletId, const QString iAddress, const QByteArray iPayload)
{
    QMap< QString, QByteArray > lPayloads;
    lPayloads[iMicroWalletId] = iPayload;
    return mPriv->microWalletCreates(lPayloads, iAddress);
}

bool Database::microWalletCreate(const QString iMicroWalletId, const QByteArray iPayload, const QString iAddress)
{
    QMap< QString, QByteArray > lPayloads;
    lPayloads[iMicroWalletId] = iPayload;
    return mPriv->microWalletCreates(lPayloads, iAddress);
}

bool Database::microWalletCreates(const QMap<QString, QByteArray> iMicroWalletIdsAndPayloads, const QString iAddress)
{ return mPriv->microWalletCreates(iMicroWalletIdsAndPayloads,iAddress); }

QByteArray Database::microWalletCopyPayload(const QString iMicroWalletId, const QString iAddress)
{
    QMap< QString, QByteArray > lPayloads = mPriv->microWalletsCopyPayload(QStringList() << iMicroWalletId,iAddress);
    return lPayloads[iMicroWalletId];
}

QMap<QString, QByteArray> Database::microWalletsCopyPayload(const QString iAddress, const QStringList iMicroWalletIds)
{ return mPriv->microWalletsCopyPayload(iMicroWalletIds,iAddress); }

QMap<QString, QByteArray> Database::microWalletsCopyPayload(const QStringList iMicroWalletIds, const QString iAddress)
{ return mPriv->microWalletsCopyPayload(iMicroWalletIds,iAddress); }

bool Database::microWalletDelete(const QString iMicroWalletId, const QString iAddress)
{ return mPriv->microWalletsDelete(QStringList() << iMicroWalletId, iAddress); }

bool Database::microWalletsDelete(const QString iAddress, const QStringList iMicroWalletIds)
{ return mPriv->microWalletsDelete(iMicroWalletIds,iAddress); }

bool Database::microWalletsDelete(const QStringList iMicroWalletIds, const QString iAddress)
{ return mPriv->microWalletsDelete(iMicroWalletIds,iAddress); }
