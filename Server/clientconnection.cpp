#include "clientconnection.h"
#include "rpcserverhandler.h"
#include "rpcmessage.h"
#include "rpcmessagepingrequest.h"
#include "rpcmessagepingreply.h"

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
    connect( this, &ClientConnection::clientDisconnected, iServer, &RPCServerHandler::clientDisconnected );

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
        QString lRawMessage = mConnection->nextTextMessage();
        if( ! lRawMessage.isEmpty() ) {
            RPCMessage  lMessage{lRawMessage};
            QString     lCommand = lMessage.fieldValue(QStringLiteral(kFieldKey_Command)).toString();

            if( lCommand == RPCMessagePingRequest::commandValue() ) {
                RPCMessagePingRequest   lPingReq{lMessage};
                QString lReply = RPCMessagePingReply::create(lPingReq.payload(),QStringLiteral("Threshodl"),"",RPCMessage::KeyEncoding::None);
                if( ! lReply.isEmpty() ) {
                    mConnection->sendTextMessage(lReply);
                }else{
                    qWarning() << "Failed to generate a reply message, this is a coding issue!";
                }
            }else{
                qWarning() << "Unknown RPC command received from client, this could be a hack attempt or a corrupt message!" << lMessage.fieldValue(QStringLiteral(kFieldKey_Command)).toString();
            }
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

QString ClientConnection::username() const
{
    if( mAuthenticated ) return mUsername;
    return QString();
}
