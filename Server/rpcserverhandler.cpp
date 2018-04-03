#include "rpcserverhandler.h"
#include "rpcserver.h"

#include <QDebug>

RPCServerHandler::RPCServerHandler(QObject *iParent)
    : QObject(iParent)
{

}

void RPCServerHandler::serverStarted()
{
    qInfo() << "RPC Websocket server started.";
}

void RPCServerHandler::serverFailedToStart()
{
    qFatal("Unable to start RPC Websocket server!");
}

void RPCServerHandler::newConnectionArrived()
{
    qInfo() << "Received new client connection notification";
    auto lServer = dynamic_cast<RPCServer *>(sender());
    if( lServer ) {
        auto lConnection = lServer->nextConnection();
        if( lConnection ) {
            qInfo() << "Activating new client connection.";
            mActiveClientConnections << QSharedPointer<ClientConnection>(new ClientConnection(lConnection,this));
        }else{
            qInfo() << "Recieved stale new client connection notification, ignoring.";
        }
    }else{
        qInfo() << "New client connection notification not from a valid signal source!";
    }
}
