#ifdef ENABLE_RPCSERVER

#include "rpcserver.h"
#include "rpcconnection.h"

RPCServer::RPCServer(QObject *iParent) : QObject(iParent)
{ }

RPCServer::~RPCServer()
{
    stopListening();
}

bool RPCServer::isListening() const
{
    return (mSocketServer != nullptr && mSocketServer->isListening());
}

RPCConnection *RPCServer::nextConnection()
{
    if( isListening() ) {
        QWebSocket * lSocket = mSocketServer->nextPendingConnection();
        if( lSocket ) {
            RPCConnection * lConnection = new RPCConnection(lSocket, this);
            mConnectionsLock.lock();
            mConnections << lConnection;
            mConnectionsLock.unlock();
            return lConnection;
        }
    }

    return nullptr;
}

void RPCServer::startListening(quint16 iPort, const QString &iServerName, const QSslConfiguration &iSslConfiguration, const QHostAddress &iAddress)
{
    try {
        if( isListening() ) throw __LINE__;
        if( mSocketServer ) throw __LINE__;

        mSocketServer       = new QWebSocketServer(iServerName,QWebSocketServer::SecureMode);
        mServerName         = iServerName;
        mSslConfiguration   = iSslConfiguration;
        mPort               = iPort;
        mHostAddress        = iAddress;

        if( mPort < 1 ) throw __LINE__;

        connect( mSocketServer, &QWebSocketServer::newConnection, this, &RPCServer::newConnection );

        mSocketServer->setMaxPendingConnections(100);
        mSocketServer->setSslConfiguration(mSslConfiguration);
        mSocketServer->setServerName(mServerName);
        if( ! mSocketServer->listen(mHostAddress,mPort) ) throw __LINE__;

        emit startedListening();
    } catch(...) {
        emit failedToStartListening();
    }
}

void RPCServer::startListening(const QHostAddress &iAddress, quint16 iPort, const QString &iServerName, const QSslConfiguration &iSslConfiguration)
{ startListening(iPort, iServerName, iSslConfiguration, iAddress); }

void RPCServer::stopListening()
{
    try {
        if( ! isListening() ) throw __LINE__;
        if( mSocketServer->thread() == thread() ) {
            mSocketServer->deleteLater();
        } else {
            QMetaObject::invokeMethod(mSocketServer,"deleteLater",Qt::QueuedConnection);
        }
        mSocketServer = nullptr;

        emit stoppedListening();
    } catch(...) {
        emit failedToStopListening();
    }
}

#endif // ENABLE_RPCSERVER
