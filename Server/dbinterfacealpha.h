#ifndef DBALPHA_H
#define DBALPHA_H

#include "dbinterface.h"

#include <QMutex>
#include <QString>
#include <QSqlDatabase>

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

    void setSqlType( const QString iSqlType = QStringLiteral("QPSQL") );
    QString sqlType() const;

    bool initDB();
    bool addressExists( const QString iAddress );
    bool addressCreate( const QString iAddress, const QByteArray iPublicKey );
};

#endif // DBALPHA_H
