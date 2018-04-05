#include "clientconnection.h"
#include "rpcserverhandler.h"
#include "rpcmessage.h"
#include "clientalphahandler.h"

//#include "rpcmessagepingrequest.h"
//#include "rpcmessagepingreply.h"
//#include "rpcmessagecreateaccountrequest.h"

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
    connect( iConnection, &RPCConnection::socketError, this, &ClientConnection::socketError );
    connect( iConnection, &RPCConnection::sslErrors, this, &ClientConnection::sslErrors );
    connect( this, &ClientConnection::clientDisconnected, iServer, &RPCServerHandler::clientDisconnected );

    mCheckBufferTimer->start();
}

bool ClientConnection::sendMessage(const QString &iMessage)
{ return mConnection->sendTextMessage(iMessage); }

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
        QString lRawMessage = mConnection->nextTextMessage();
        if( ! lRawMessage.isEmpty() ) {
            RPCMessage  lMessage{lRawMessage};
            QString     lCommand = lMessage.fieldValue(QStringLiteral(kFieldKey_Command)).toString();

            if( ! ClientAlphaHandler::handle(this,lCommand,lMessage) ) {
                qWarning() << "Unknown RPC command received from client, this could be a hack attempt or a corrupt message!" << lMessage.fieldValue(QStringLiteral(kFieldKey_Command)).toString();
                mConnection->close();
            }

//            if( lCommand == RPCMessagePingRequest::commandValue() ) {
//                RPCMessagePingRequest   lPingReq{lMessage};
//                QString lReply = RPCMessagePingReply::create(lPingReq.payload(),QStringLiteral("Threshodl"));
//                if( ! lReply.isEmpty() ) {
//                    mConnection->sendTextMessage(lReply);
//                }else{
//                    qWarning() << "Failed to generate a reply message, this is a coding issue!";
//                }
//            }else if( lCommand == RPCMessageCreateAccountRequest::commandValue() ) {

//            }else{
//                qWarning() << "Unknown RPC command received from client, this could be a hack attempt or a corrupt message!" << lMessage.fieldValue(QStringLiteral(kFieldKey_Command)).toString();
//            }
        }
    }
}

void ClientConnection::outgoingMessageSent()
{
    // Nothing to do for now?
}

void ClientConnection::outgoingMessageFailedToSend()
{
    mConnection->close();
}

void ClientConnection::socketError(QAbstractSocket::SocketError iError)
{
    Q_UNUSED(iError)
    mConnection->close();
}

void ClientConnection::sslErrors(const QList<QSslError> iErrors)
{
    Q_UNUSED(iErrors)
    mConnection->close();
}

QString ClientConnection::username() const
{
    if( mAuthenticated ) return mUsername;
    return QString();
}
