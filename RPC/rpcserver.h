#ifndef RPCSERVER_H
#define RPCSERVER_H

#include "rpc.h"

#ifdef ENABLE_RPCSERVER

#include <QString>
#include <QMutex>
#include <QList>
#include <QHostAddress>
#include <QSslConfiguration>
#include <QObject>
#include <QWebSocketServer>

class RPCServer : public QObject
{
    Q_OBJECT
public:
    explicit RPCServer(QObject *iParent = nullptr);
    ~RPCServer();

    bool isListening() const;
    RPCConnection * nextConnection();

signals:
    void failedToStartListening();
    void startedListening();
    void failedToStopListening();
    void stoppedListening();
    void newConnection();

public slots:
    void startListening(quint16 iPort, const QString &iServerName, const QSslConfiguration &iSslConfiguration, const QHostAddress &iAddress = QHostAddress::Any);
    void startListening(const QHostAddress &iAddress, quint16 iPort, const QString &iServerName, const QSslConfiguration &iSslConfiguration);
    void stopListening();

private:
    quint16                 mPort               = 0;
    QString                 mServerName;
    QHostAddress            mHostAddress        = QHostAddress::Any;
    QSslConfiguration       mSslConfiguration;
    QWebSocketServer *      mSocketServer       = nullptr;
    QMutex                  mConnectionsLock;
    QList<RPCConnection *>  mConnections;
};

#endif // ENABLE_RPCSERVER

#endif // RPCSERVER_H
