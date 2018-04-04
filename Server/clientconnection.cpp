#include "clientconnection.h"
#include "rpcserverhandler.h"
#include "rpcmessage.h"
#include "rpcmessagepingrequest.h"

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
            if( lMessage.fieldValue(QStringLiteral(kFieldKey_Command)).toString() == RPCMessagePingRequest::commandValue() ) {
                RPCMessagePingRequest   lPingReq{lMessage};
                QString lReply = RPCMessage::toMessage(
                            QList<RPCField>()
                            << RPCField(RPCMessagePingRequest::pingPayloadKey(),lPingReq.payload())
                            << RPCField(QStringLiteral(kFieldKey_Command),RPCMessagePingRequest::commandValue()),
                            QStringLiteral("Threshodl"), QByteArray(), RPCMessage::KeyEncoding::None);

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

}

void ClientConnection::outgoingMessageFailedToSend()
{

}

QString ClientConnection::username() const
{
    if( mAuthenticated ) return mUsername;
    return QString();
}
