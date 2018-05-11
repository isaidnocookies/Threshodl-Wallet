#include "wcpserverhandlerinterface.h"

void WCPServerHandlerInterface::_addActiveClient(WCPClientInterface *iClient)
{
    mActiveClientsLock.lock();
    mActiveClients.removeAll(iClient);
    mActiveClients.append(iClient);
    mActiveClientsLock.unlock();
}

void WCPServerHandlerInterface::setWCPServer(WCPServer *iServer)
{
    mServer = iServer;
    connect( mServer, &WCPServer::startedListening, this, &WCPServerHandlerInterface::serverStarted );
    connect( mServer, &WCPServer::stoppedListening, this, &WCPServerHandlerInterface::serverStopped );
    connect( mServer, &WCPServer::failedToStartListening, this, &WCPServerHandlerInterface::serverFailedToStart );
    connect( mServer, &WCPServer::failedToStopListening, this, &WCPServerHandlerInterface::serverFailedToStop );
    connect( mServer, &WCPServer::newConnection, this, &WCPServerHandlerInterface::newConnectionArrived );
}

void WCPServerHandlerInterface::clientDisconnected(WCPClientInterface *iClient)
{
    mActiveClientsLock.lock();
    mActiveClients.removeAll(iClient);
    mActiveClientsLock.unlock();
}
