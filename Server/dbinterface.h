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
    virtual bool addressExists( const QString iAddress )                                = 0;
    virtual bool addressCreate( const QString iAddress, const QByteArray iPublicKey )   = 0;
    virtual bool addressValidate( const QString iAddress, const QByteArray iPublicKey ) = 0;
    virtual bool addressDelete( const QString iAddress )                                = 0;
};

#endif // DBINTERFACE_H
