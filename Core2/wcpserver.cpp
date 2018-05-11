#include "wcpserver.h"

WCPServer::WCPServer(QObject *iParent) : QObject(iParent)
{ }

WCPServer::~WCPServer()
{
    stopListening();
}

bool WCPServer::isListening() const
{
    return (mSocketServer != nullptr && mSocketServer->isListening());
}

WCPConnection *WCPServer::nextConnection()
{
    if( isListening() ) {
        QWebSocket * lSocket = mSocketServer->nextPendingConnection();
        if( lSocket ) {
            WCPConnection * lConnection = new WCPConnection(lSocket, this);
            mConnectionsLock.lock();
            mConnections << lConnection;
            mConnectionsLock.unlock();
            return lConnection;
        }
    }

    return nullptr;
}

void WCPServer::startListening(quint16 iPort, const QString &iServerName, const QSslConfiguration &iSslConfiguration, const QHostAddress &iAddress)
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

        // connect( mSocketServer, &QWebSocketServer::newConnection, this, &WCPServer::newConnection );
        connect( mSocketServer, &QWebSocketServer::newConnection, [this](){
            emit newConnection();
        });

        mSocketServer->setMaxPendingConnections(100);
        mSocketServer->setSslConfiguration(mSslConfiguration);
        mSocketServer->setServerName(mServerName);
        if( ! mSocketServer->listen(mHostAddress,mPort) ) throw __LINE__;

        emit startedListening();
    } catch(...) {
        emit failedToStartListening();
    }
}

void WCPServer::startListening(const QHostAddress &iAddress, quint16 iPort, const QString &iServerName, const QSslConfiguration &iSslConfiguration)
{ startListening(iPort, iServerName, iSslConfiguration, iAddress); }

void WCPServer::stopListening()
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
