#include "wcpserverhandlerinterface.h"

void WCPServerHandlerInterface::setWCPServer(WCPServer *iServer)
{
    mServer = iServer;
    connect( mServer, &WCPServer::startedListening, this, &WCPServerHandlerInterface::serverStarted );
    connect( mServer, &WCPServer::stoppedListening, this, &WCPServerHandlerInterface::serverStopped );
    connect( mServer, &WCPServer::failedToStartListening, this, &WCPServerHandlerInterface::serverFailedToStart );
    connect( mServer, &WCPServer::failedToStopListening, this, &WCPServerHandlerInterface::serverFailedToStop );
    connect( mServer, &WCPServer::newConnection, this, &WCPServerHandlerInterface::newConnectionArrived );
}
