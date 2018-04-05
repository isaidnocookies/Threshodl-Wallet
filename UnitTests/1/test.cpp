#include "test.h"
#include "rpcmessagepingrequest.h"

Test::Test(RPCConnection * iConnection, QObject *iParent)
    : QObject(iParent)
    , mConnection(iConnection)
{
    connect( mConnection, &RPCConnection::connected, this, &Test::rpcConnected );
    connect( mConnection, &RPCConnection::socketError, this, &Test::rpcSocketError );
    connect( mConnection, &RPCConnection::sslErrors, this, &Test::rpcSslErrors );
    connect( mConnection, &RPCConnection::textMessageReceived, this, &Test::messageReceived );
    connect( mConnection, &RPCConnection::failedToSendTextMessage, this, &Test::failedToSendMessage );
    connect( mConnection, &RPCConnection::sentTextMessage, this, &Test::sentMessage );

    qDebug() << RPCMessagePingRequest::create("ping", QStringLiteral("UnitTest"), "", RPCMessage::KeyEncoding::None);
}

void Test::rpcConnected()
{
    qDebug() << "Connected.";
    mConnection->sendTextMessage( RPCMessagePingRequest::create("ping", QStringLiteral("UnitTest"), "", RPCMessage::KeyEncoding::None) );
}

void Test::rpcSslErrors(const QList<QSslError> iErrors)
{
    qDebug() << "Received" << iErrors.size() << "Ssl errors:";
    int lIndex = 0;
    for( auto lError : iErrors ) {
        qDebug() << "[" << lIndex++ << "]" << lError.errorString();
    }
}

void Test::rpcSocketError(QAbstractSocket::SocketError iError)
{
    qDebug() << "Socket error" << iError;
}

void Test::messageReceived()
{
    qDebug() << "Received message:" << mConnection->nextTextMessage();
}

void Test::failedToSendMessage()
{
    qDebug() << "Failed to send message.";
}

void Test::sentMessage()
{
    qDebug() << "Sent message.";
}
