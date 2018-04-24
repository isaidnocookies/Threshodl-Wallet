#ifndef DBINTERFACEV1_H
#define DBINTERFACEV1_H

#include "dbinterface.h"

#include <QMutex>
#include <QString>
#include <QSqlDatabase>

class DBInterfaceV1 : public DBInterface
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
    enum EscrowRecordState {
        Invalid     = 0x0,
        Unlocked    = 0x1,
        Locked      = 0x2
    };

    typedef enum EscrowRecordState EscrowRecordState;

    inline quint64 _nextConnectionCounter()
    { QMutexLocker l{&mConnectionCounterLock}; return ++mConnectionCounter; }

    inline QString _nextConnectionCounterString()
    { return (mLastConnectionCounterString = QStringLiteral("%1").arg(_nextConnectionCounter())); }

    bool _connected();
    bool _connectOrReconnectToDB(QSqlDatabase &oDatabase);
    bool _connectToDB(QString &oConnectionCounterString, QSqlDatabase &oDatabase);
    bool _reconnectToDB(const QString iConnectionCounterString, QSqlDatabase &oDatabase);
    bool _beginTransaction(QSqlDatabase &iDatabase);
    bool _beginTransactionLockTable(QSqlDatabase &iDatabase, const QString iTableName, bool iLockExclusive = true);
    bool _beginTransactionLockTables(QSqlDatabase &iDatabase, const QStringList iTableNames, bool iLockExclusive = true);
    bool _commitTransaction(QSqlDatabase &iDatabase);
    bool _commitTransactionUnlockTable(QSqlDatabase &iDatabase, const QString iTableName);
    bool _commitTransactionUnlockTables(QSqlDatabase &iDatabase, const QStringList iTableNames);
    bool _rollbackTransaction(QSqlDatabase &iDatabase);
    void _closeDB(const QString iConnectionCounterString, QSqlDatabase &iDatabase);

    QString lastConnectionCounterString() const;

    QString _sanitizeAccountName(const QString iAccountName);
    QString _escrowTableNameForAccount(const QString iAccountName);
    QSqlQuery _createEscrowTableSqlQueryForAccount(const QString iAccountName, QSqlDatabase iDatabase);

public:
    DBInterfaceV1( const QString iUserName, const QString iPassword, const QString iDatabaseName, const QString iHostName, quint16 iPort = 5432 );

    void setSqlType( const QString iSqlType = QStringLiteral("QPSQL") ) override;
    QString sqlType() const override;

    bool initDB() override;

    bool addressExists( const QString iAddress ) override;
    bool addressCreate( const QString iAddress, const QByteArray iPublicKey ) override;
    bool addressValidate( const QString iAddress, const QByteArray iPublicKey ) override;
    bool addressDelete( const QString iAddress ) override;
    QByteArray publicKeyForAddress(const QString iAddress) override;

    bool microWalletExists( const QString iMicroWalletId ) override;
    bool microWalletOwnershipCheck( const QString iMicroWalletId, const QString iAddress ) override;
    bool microWalletChangeOwnership( const QString iMicroWalletId, const QString iFromAddress, const QString iToAddress ) override;
    bool microWalletChangeOwnership(const QStringList iMicroWalletIds, const QString iFromAddress, const QString iToAddress ) override;
    bool microWalletCreate( const QString iMicroWalletId, const QString iAddress, const QByteArray iPayload ) override {return false;}
    QByteArray microWalletCopyPayload( const QString iMicroWalletId, const QString iAddress ) override {return QByteArray();}
    bool microWalletDelete( const QString iMicroWalletId, const QString iAddress ) override {return false;}
};

#endif // DBINTERFACEV1_H
