#include "database.h"
#include "modulelinker.h"
#include "app.h"
#include "databasepriv.h"
#include "databaseprivpsql.h"

static DataBaseML _gRegisterModuleLinker;

DataBaseML::DataBaseML()
{
    ModuleLinker::registerModule(QStringLiteral("DataBase-1"),DataBaseML::creator,DataBaseML::doInit,DataBaseML::start,DataBaseML::startInOwnThread);
}

void *DataBaseML::creator(void *pointerToAppObject)
{
    App *       lApp    = reinterpret_cast<App *>(pointerToAppObject);
    if( lApp ) {
        auto    lConfig = lApp->configuration();
        QString lDBType = QStringLiteral("QPSQL");

        if( lConfig->contains(QStringLiteral("DataBaseType")) )
            lDBType = lConfig->toString(QStringLiteral("DataBaseType"));
        else if( lConfig->contains(QStringLiteral("DBType")) )
            lDBType = lConfig->toString(QStringLiteral("DBType"));

        DataBase *  lDB     = new DataBase{lDBType};
        lDB->mApp = lApp;
        return lDB;
    }

    return nullptr;
}

bool DataBaseML::doInit(void *pointerToThis, void *pointerToAppObject)
{
    Q_UNUSED(pointerToAppObject)
    DataBase *  lDB = reinterpret_cast<DataBase *>(pointerToThis);
    if( lDB )
        return lDB->createTables();
    return false;
}

bool DataBaseML::startInOwnThread()
{ return false; }

bool DataBaseML::start(void *pointerToThis, void *pointerToAppObject)
{
    Q_UNUSED(pointerToAppObject)
    DataBase *  lDB = reinterpret_cast<DataBase *>(pointerToThis);
    if( lDB ) {
        return lDB->_createPriv();
    }
    return false;
}

bool DataBase::_createPriv()
{
    QString lType = databaseType().toLower();

    if( lType == QStringLiteral("psql") || lType == QStringLiteral("qpsql") ) {
        DataBasePrivPSQL *  lPriv   = new DataBasePrivPSQL;
        lPriv->mApp                 = mApp;
        mPriv                       = dynamic_cast<DataBasePriv *>(lPriv);
        return lPriv->_init();
    }

    return false;
}

DataBase::DataBase(const QString iDatabaseType) : DataBaseInterface(iDatabaseType)
{ }

DataBase::~DataBase()
{
    if( mPriv )
        delete mPriv;
}

bool DataBase::createTables()
{
    if( ! mPriv ) {
        if( ! _createPriv() )
            return false;
    }

    return mPriv->createTables();
}

bool DataBase::addressExists(const QString iAddress)
{ return mPriv->addressExists(iAddress); }

bool DataBase::addressCreate(const QString iAddress, const QByteArray iPublicKey)
{ return mPriv->addressCreate(iAddress,iPublicKey); }

bool DataBase::addressValidate(const QString iAddress, const QByteArray iPublicKey)
{ return mPriv->addressValidate(iAddress,iPublicKey); }

bool DataBase::addressDelete(const QString iAddress)
{ return mPriv->addressDelete(iAddress); }

QByteArray DataBase::publicKeyForAddress(const QString iAddress)
{ return mPriv->publicKeyForAddress(iAddress); }

bool DataBase::microWalletExists(const QString iMicroWalletId)
{ return mPriv->microWalletsExists(QStringList() << iMicroWalletId); }

bool DataBase::microWalletsExists(const QStringList iMicroWalletIds)
{ return mPriv->microWalletsExists(iMicroWalletIds); }

bool DataBase::microWalletOwnershipCheck(const QString iMicroWalletId, const QString iAddress)
{ return mPriv->microWalletsOwnershipCheck(QStringList() << iMicroWalletId, iAddress); }

bool DataBase::microWalletsOwnershipCheck(const QStringList iMicroWalletIds, const QString iAddress)
{ return mPriv->microWalletsOwnershipCheck(iMicroWalletIds,iAddress); }

bool DataBase::microWalletChangeOwnership(const QString iMicroWalletId, const QString iFromAddress, const QString iToAddress)
{ return mPriv->microWalletsChangeOwnership(QStringList() << iMicroWalletId, iFromAddress, iToAddress); }

bool DataBase::microWalletsChangeOwnership(const QStringList iMicroWalletIds, const QString iFromAddress, const QString iToAddress)
{ return mPriv->microWalletsChangeOwnership(iMicroWalletIds, iFromAddress, iToAddress); }

bool DataBase::microWalletCreate(const QString iMicroWalletId, const QString iAddress, const QByteArray iPayload)
{
    QMap< QString, QByteArray > lPayloads;
    lPayloads[iMicroWalletId] = iPayload;
    return mPriv->microWalletCreates(lPayloads, iAddress);
}

bool DataBase::microWalletCreate(const QString iMicroWalletId, const QByteArray iPayload, const QString iAddress)
{
    QMap< QString, QByteArray > lPayloads;
    lPayloads[iMicroWalletId] = iPayload;
    return mPriv->microWalletCreates(lPayloads, iAddress);
}

bool DataBase::microWalletCreates(const QMap<QString, QByteArray> iMicroWalletIdsAndPayloads, const QString iAddress)
{ return mPriv->microWalletCreates(iMicroWalletIdsAndPayloads,iAddress); }

QByteArray DataBase::microWalletCopyPayload(const QString iMicroWalletId, const QString iAddress)
{
    QMap< QString, QByteArray > lPayloads = mPriv->microWalletsCopyPayload(QStringList() << iMicroWalletId,iAddress);
    return lPayloads[iMicroWalletId];
}

QMap<QString, QByteArray> DataBase::microWalletsCopyPayload(const QString iAddress, const QStringList iMicroWalletIds)
{ return mPriv->microWalletsCopyPayload(iMicroWalletIds,iAddress); }

QMap<QString, QByteArray> DataBase::microWalletsCopyPayload(const QStringList iMicroWalletIds, const QString iAddress)
{ return mPriv->microWalletsCopyPayload(iMicroWalletIds,iAddress); }

bool DataBase::microWalletDelete(const QString iMicroWalletId, const QString iAddress)
{ return mPriv->microWalletsDelete(QStringList() << iMicroWalletId, iAddress); }

bool DataBase::microWalletsDelete(const QString iAddress, const QStringList iMicroWalletIds)
{ return mPriv->microWalletsDelete(iMicroWalletIds,iAddress); }

bool DataBase::microWalletsDelete(const QStringList iMicroWalletIds, const QString iAddress)
{ return mPriv->microWalletsDelete(iMicroWalletIds,iAddress); }
