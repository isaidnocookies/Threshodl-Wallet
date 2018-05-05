#ifndef DATABASE_H
#define DATABASE_H

#include "../../Interface/DatabaseInterface/databaseinterface.h"

class App;
class DataBaseML;
class DataBasePriv;
class DataBase : public DataBaseInterface
{
    friend class DataBaseML;
protected:
    App *           mApp    = nullptr;
    DataBasePriv *  mPriv   = nullptr;

    bool            _createPriv();

public:
    DataBase(const QString iDatabaseType = QStringLiteral("NotSet"));
    virtual ~DataBase();

    bool                        createTables() override;
    // Relating to user address accounts:
    bool                        addressExists( const QString iAddress ) override;
    bool                        addressCreate( const QString iAddress, const QByteArray iPublicKey ) override;
    bool                        addressValidate( const QString iAddress, const QByteArray iPublicKey ) override;
    bool                        addressDelete( const QString iAddress ) override;
    QByteArray                  publicKeyForAddress(const QString iAddress) override;

    // Relating to microwallets:
    bool                        microWalletExists( const QString iMicroWalletId ) override;
    bool                        microWalletsExists( const QStringList iMicroWalletIds ) override;

    bool                        microWalletOwnershipCheck( const QString iMicroWalletId, const QString iAddress ) override;
    bool                        microWalletsOwnershipCheck( const QStringList iMicroWalletIds, const QString iAddress ) override;

    bool                        microWalletChangeOwnership( const QString iMicroWalletId, const QString iFromAddress, const QString iToAddress ) override;
    bool                        microWalletsChangeOwnership( const QStringList iMicroWalletIds, const QString iFromAddress, const QString iToAddress ) override;

    bool                        microWalletCreate( const QString iMicroWalletId, const QString iAddress, const QByteArray iPayload ) override;
    bool                        microWalletCreate( const QString iMicroWalletId, const QByteArray iPayload, const QString iAddress ) override;
    bool                        microWalletCreates( const QMap< QString, QByteArray> iMicroWalletIdsAndPayloads, const QString iAddress ) override;

    QByteArray                  microWalletCopyPayload( const QString iMicroWalletId, const QString iAddress ) override;
    QMap< QString, QByteArray>  microWalletsCopyPayload( const QString iAddress, const QStringList iMicroWalletIds ) override;
    QMap< QString, QByteArray>  microWalletsCopyPayload( const QStringList iMicroWalletIds, const QString iAddress ) override;

    bool                        microWalletDelete( const QString iMicroWalletId, const QString iAddress ) override;
    bool                        microWalletsDelete( const QString iAddress, const QStringList iMicroWalletIds ) override;
    bool                        microWalletsDelete( const QStringList iMicroWalletIds, const QString iAddress ) override;
};

class DataBaseML
{
public:
    DataBaseML();
    static void * creator(void * pointerToAppObject);                                      // Returns a pointer to a new object
    static bool doInit(void * pointerToThis, void * pointerToAppObject);                   // Returns true on DoInit success
    static bool startInOwnThread();                                                        // Returns true if should be created and started in own thread
    static bool start(void * pointerToThis, void * pointerToAppObject);                    // Returns true on Start success
};

#endif // DATABASE_H
