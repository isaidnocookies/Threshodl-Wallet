#ifndef DATABASEPRIV_H
#define DATABASEPRIV_H

#include <QByteArray>
#include <QString>
#include <QList>
#include <QMap>
#include <QStringList>

class DatabasePriv
{
public:
    virtual ~DatabasePriv() { }

    virtual bool                        createTables()                                                                                                                                  = 0;
    virtual bool                        addressExists( const QString iAddress )                                                                                                         = 0;
    virtual bool                        addressCreate( const QString iAddress, const QByteArray iPublicKey )                                                                            = 0;
    virtual bool                        addressValidate( const QString iAddress, const QByteArray iPublicKey )                                                                          = 0;
    virtual bool                        addressDelete( const QString iAddress )                                                                                                         = 0;
    virtual QByteArray                  publicKeyForAddress(const QString iAddress)                                                                                                     = 0;
    virtual bool                        microWalletAcquireFreeWalletIdPrefixBlock(unsigned int iBlockSize, QString &oStartingWalletIdPrefix)                                            = 0;
    virtual bool                        microWalletsExists( const QStringList iMicroWalletIds )                                                                                         = 0;
    virtual bool                        microWalletsOwnershipCheck( const QStringList iMicroWalletIds, const QString iAddress )                                                         = 0;
    virtual bool                        microWalletsChangeOwnership( const QStringList iMicroWalletIds, const QString iFromAddress, const QString iToAddress )                          = 0;
    virtual bool                        microWalletScratchCreates( const QMap< QString, QByteArray> iMicroWalletIdsAndPayloads, const QString iAddress, const quint64 iCreationTime )   = 0;
    virtual bool                        microWalletMoveFromScratch( const QStringList iMicroWalletIds, const QString iAddress )                                                         = 0;
    virtual QMap< QString, QByteArray>  microWalletsCopyPayload( const QStringList iMicroWalletIds, const QString iAddress )                                                            = 0;
    virtual bool                        microWalletsDelete( const QStringList iMicroWalletIds, const QString iAddress )                                                                 = 0;
};

#endif // DATABASEPRIV_H
