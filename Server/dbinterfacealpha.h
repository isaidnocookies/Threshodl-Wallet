#ifndef DBALPHA_H
#define DBALPHA_H

#include "dbinterface.h"

#include <QMutex>
#include <QString>
#include <QSqlDatabase>

////////////////////////////////////////
///
///
///  LOCKING IS MISSING, SEE: https://nordeus.com/blog/engineering/postgresql-locking-revealed/
///
///
////////////////////////////////////////

class DBInterfaceAlpha : public DBInterface
{
protected:
    QMutex      mConnectionCounterLock;
    quint64     mConnectionCounter              = 0;
    QString     mSqlType                        = QStringLiteral("QPSQL");

    QString     mLastConnectionCounterString    = QStringLiteral("0");

    QString     mUserName;
    QString     mPassword;
    QString     mDatabaseName;
    QString     mHostName;
    quint16     mPort                           = 5432;

protected:
    inline quint64 _nextConnectionCounter()
    { QMutexLocker l{&mConnectionCounterLock}; return ++mConnectionCounter; }

    inline QString _nextConnectionCounterString()
    { return (mLastConnectionCounterString = QStringLiteral("%1").arg(_nextConnectionCounter())); }

    bool _connected();
    bool _connectOrReconnectToDB(QSqlDatabase &oDatabase);
    bool _connectToDB(QString &oConnectionCounterString, QSqlDatabase &oDatabase);
    bool _reconnectToDB(const QString iConnectionCounterString, QSqlDatabase &oDatabase);
    bool _flushDB(QSqlDatabase &iDatabase);
    void _closeDB(const QString iConnectionCounterString, QSqlDatabase &iDatabase);

    QString lastConnectionCounterString() const;

public:
    DBInterfaceAlpha( const QString iUserName, const QString iPassword, const QString iDatabaseName, const QString iHostName, quint16 iPort = 5432 );

    void setSqlType( const QString iSqlType = QStringLiteral("QPSQL") ) override;
    QString sqlType() const override;

    bool initDB() override;

    bool addressExists( const QString iAddress ) override;
    bool addressCreate( const QString iAddress, const QByteArray iPublicKey ) override;
    bool addressValidate( const QString iAddress, const QByteArray iPublicKey ) override;
    bool addressDelete( const QString iAddress ) override;
    QByteArray publicKeyForAddress(const QString iAddress) override;

    bool microWalletExists( const QString iMicroWalletId ) override;
    bool microWalletExists(const QStringList iMicroWalletId) override
    { Q_UNUSED(iMicroWalletId) return false; } // This was added after ALPHA
    bool microWalletOwnershipCheck( const QString iMicroWalletId, const QString iAddress ) override;
    bool microWalletOwnershipCheck( const QStringList iMicroWalletIds, const QString iAddress ) override
    { Q_UNUSED(iMicroWalletIds) Q_UNUSED(iAddress) return false; } // This was added after ALPHA
    bool microWalletChangeOwnership( const QString iMicroWalletId, const QString iFromAddress, const QString iToAddress ) override;
    bool microWalletChangeOwnership( const QStringList iMicroWalletIds, const QString iFromAddress, const QString iToAddress ) override
    { Q_UNUSED(iMicroWalletIds) Q_UNUSED(iFromAddress) Q_UNUSED(iToAddress) return false; } // This was added after ALPHA
    bool microWalletCreate( const QString iMicroWalletId, const QString iAddress, const QByteArray iPayload ) override;
    QByteArray microWalletCopyPayload( const QString iMicroWalletId, const QString iAddress ) override;
    bool microWalletDelete( const QString iMicroWalletId, const QString iAddress ) override;
};

#endif // DBALPHA_H
