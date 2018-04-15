#ifndef DBINTERFACE_H
#define DBINTERFACE_H

#include <QByteArray>
#include <QString>

class DBInterface
{
public:
    DBInterface() { }

    virtual void setSqlType( const QString iSqlType = QStringLiteral("QPSQL") )         = 0;
    virtual QString sqlType() const                                                     = 0;

    virtual bool initDB()                                                               = 0;

    // Relating to user address accounts:
    virtual bool addressExists( const QString iAddress )                                = 0;
    virtual bool addressCreate( const QString iAddress, const QByteArray iPublicKey )   = 0;
    virtual bool addressValidate( const QString iAddress, const QByteArray iPublicKey ) = 0;
    virtual bool addressDelete( const QString iAddress )                                = 0;
    virtual QByteArray publicKeyForAddress(const QString iAddress)                      = 0;


    // Relating to microwallets:
    virtual bool microWalletExists( const QString iMicroWalletId )                                                                  = 0;
    virtual bool microWalletOwnershipCheck( const QString iMicroWalletId, const QString iAddress )                                  = 0;
    virtual bool microWalletChangeOwnership( const QString iMicroWalletId, const QString iFromAddress, const QString iToAddress )   = 0;
    virtual bool microWalletCreate( const QString iMicroWalletId, const QString iAddress, const QByteArray iPayload )               = 0;
    virtual QByteArray microWalletCopyPayload( const QString iMicroWalletId, const QString iAddress )                               = 0;
    virtual bool microWalletDelete( const QString iMicroWalletId, const QString iAddress )                                          = 0;
};

#endif // DBINTERFACE_H
