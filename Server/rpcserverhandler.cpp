#include "rpcserverhandler.h"

#include <QDebug>

RPCServerHandler::RPCServerHandler(QObject *iParent) : QObject(iParent)
{

}

void RPCServerHandler::serverStarted()
{

}

void RPCServerHandler::serverFailedToStart()
{

}

void RPCServerHandler::newConnectionArrived()
{
    qWarning() << "Connected!";
}
