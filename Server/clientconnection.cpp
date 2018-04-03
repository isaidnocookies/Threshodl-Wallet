#include "clientconnection.h"
#include "rpcserverhandler.h"

ClientConnection::ClientConnection(RPCConnection *iConnection, RPCServerHandler *iServer)
    : QObject(iServer)
    , mServer(iServer)
    , mConnection(iConnection)
{
    mCheckBufferTimer = new QTimer{this};
    mCheckBufferTimer->setInterval(1000);
    mCheckBufferTimer->setSingleShot(false);

    connect( mCheckBufferTimer, &QTimer::timeout, this, &ClientConnection::timerCheckBuffer );
    connect( iConnection, &RPCConnection::textMessageReceived, this, &ClientConnection::processIncomingMessage );
    connect( iConnection, &RPCConnection::failedToSendTextMessage, this, &ClientConnection::outgoingMessageFailedToSend );
    connect( iConnection, &RPCConnection::sentTextMessage, this, &ClientConnection::outgoingMessageSent );

    mCheckBufferTimer->start();
}

void ClientConnection::timerCheckBuffer()
{
    if( mConnection->haveTextMessages() )
        processIncomingMessage();

    if( ! mConnection->isConnected() )
        emit clientDisconnected();
}

void ClientConnection::processIncomingMessage()
{
    while( mConnection->haveTextMessages() ) {
        QString lMessage = mConnection->nextTextMessage();
    }
}

void ClientConnection::outgoingMessageSent()
{

}

void ClientConnection::outgoingMessageFailedToSend()
{

}

QString ClientConnection::username() const
{
    if( mAuthenticated ) return mUsername;
    return QString();
}
