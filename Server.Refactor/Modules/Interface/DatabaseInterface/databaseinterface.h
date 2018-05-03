#ifndef DATABASEINTERFACE_H
#define DATABASEINTERFACE_H

#include <QReadWriteLock>
#include <QByteArray>
#include <QString>

class DataBaseInterface
{
protected:
    mutable QReadWriteLock      mObjectLock;
    QString                     mDatabaseType;

public:
    DataBaseInterface(const QString iDatabaseType = QStringLiteral("NotSet"))
        : mDatabaseType(iDatabaseType)
    { }

    virtual ~DataBaseInterface()
    { }

    virtual QString                     databaseType() const                                    { QReadLocker l{&mObjectLock};  return mDatabaseType; }
    virtual void                        setDatabaseType(const QString iDatabaseType)            { QWriteLocker l{&mObjectLock}; mDatabaseType = iDatabaseType; }

    virtual bool                        createTables()                                                                                                          = 0;

    // Relating to user address accounts:
    virtual bool                        addressExists( const QString iAddress )                                                                                 = 0;
    virtual bool                        addressCreate( const QString iAddress, const QByteArray iPublicKey )                                                    = 0;
    virtual bool                        addressValidate( const QString iAddress, const QByteArray iPublicKey )                                                  = 0;
    virtual bool                        addressDelete( const QString iAddress )                                                                                 = 0;
    virtual QByteArray                  publicKeyForAddress(const QString iAddress)                                                                             = 0;

    // Relating to microwallets:
    virtual bool                        microWalletExists( const QString iMicroWalletId )                                                                       = 0;
    virtual bool                        microWalletsExists(const QStringList iMicroWalletId)                                                                    = 0;

    virtual bool                        microWalletOwnershipCheck( const QString iMicroWalletId, const QString iAddress )                                       = 0;
    virtual bool                        microWalletsOwnershipCheck( const QStringList iMicroWalletIds, const QString iAddress )                                 = 0;

    virtual bool                        microWalletChangeOwnership( const QString iMicroWalletId, const QString iFromAddress, const QString iToAddress )        = 0;
    virtual bool                        microWalletsChangeOwnership( const QStringList iMicroWalletIds, const QString iFromAddress, const QString iToAddress )  = 0;

    virtual bool                        microWalletCreate( const QString iMicroWalletId, const QString iAddress, const QByteArray iPayload )                    = 0;
    virtual bool                        microWalletCreate( const QString iMicroWalletId, const QByteArray iPayload, const QString iAddress )                    = 0;
    virtual bool                        microWalletCreates( const QMap< QString, QByteArray> iMicroWalletIdsAndPayloads, const QString iAddress )               = 0;

    virtual QByteArray                  microWalletCopyPayload( const QString iMicroWalletId, const QString iAddress )                                          = 0;
    virtual QMap< QString, QByteArray>  microWalletsCopyPayload( const QString iAddress, const QStringList iMicroWalletIds )                                    = 0;
    virtual QMap< QString, QByteArray>  microWalletsCopyPayload( const QStringList iMicroWalletIds, const QString iAddress )                                    = 0;

    virtual bool                        microWalletDelete( const QString iMicroWalletId, const QString iAddress )                                               = 0;
    virtual bool                        microWalletsDelete( const QString iAddress, const QStringList iMicroWalletIds )                                         = 0;
    virtual bool                        microWalletsDelete( const QStringList iMicroWalletIds, const QString iAddress )                                         = 0;
}

#endif // DATABASEINTERFACE_H
