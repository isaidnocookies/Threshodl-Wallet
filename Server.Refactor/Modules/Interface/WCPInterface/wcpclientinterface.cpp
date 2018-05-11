#include "wcpclientinterface.h"
#include "wcpserverhandlerinterface.h"

#include <QDebug>

WCPClientInterface::WCPClientInterface(WCPConnection *iConnection, WCPServerHandlerInterface *iServer, QObject *iParent)
    : QObject(iParent), mServer(iServer), mConnection(iConnection)
{
    mCheckBufferTimer = new QTimer{this};
    mCheckBufferTimer->setInterval(1000);
    mCheckBufferTimer->setSingleShot(false);

    connect( mCheckBufferTimer, &QTimer::timeout, this, &WCPClientInterface::timerCheckBuffer );
    connect( mConnection, &WCPConnection::textMessageReceived, this, &WCPClientInterface::processIncomingMessage );
    connect( mConnection, &WCPConnection::failedToSendTextMessage, this, &WCPClientInterface::outgoingMessageFailedToSend );
    connect( mConnection, &WCPConnection::sentTextMessage, this, &WCPClientInterface::outgoingMessageSent );
    connect( mConnection, &WCPConnection::socketError, this, &WCPClientInterface::socketError );
    connect( mConnection, &WCPConnection::sslErrors, this, &WCPClientInterface::sslErrors );
    connect( this, &WCPClientInterface::clientDisconnected, mServer, &WCPServerHandlerInterface::clientDisconnected );

    mCheckBufferTimer->start();
}

bool WCPClientInterface::sendMessage(const QString &iMessage)
{ return mConnection->sendTextMessage(iMessage); }

void WCPClientInterface::timerCheckBuffer()
{
    if( mConnection->haveTextMessages() )
        processIncomingMessage();

    if( ! mConnection->isConnected() )
        emit clientDisconnected(this);
}

void WCPClientInterface::processIncomingMessage()
{
    while( mConnection->haveTextMessages() )
    {
        QString lRawMessage = mConnection->nextTextMessage();
        if( ! lRawMessage.isEmpty() ) {
            WCPMessage  lMessage{lRawMessage};
            QString     lVersion    = lMessage.wcpVersion();
            QString     lCommand    = lMessage.fieldValue(QStringLiteral(kFieldKey_Command)).toString();
            processMessage(lVersion,lCommand,lMessage);
        }
    }
}

void WCPClientInterface::outgoingMessageSent()
{ emit messageSent(); }

void WCPClientInterface::outgoingMessageFailedToSend()
{ mConnection->close(); }

void WCPClientInterface::socketError(QAbstractSocket::SocketError iError)
{
    qDebug() << "Received a socket error" << iError;
    mConnection->close();
}

void WCPClientInterface::sslErrors(const QList<QSslError> iErrors)
{
    qWarning() << "Received SSL errors:" << iErrors;
    mConnection->close();
}
