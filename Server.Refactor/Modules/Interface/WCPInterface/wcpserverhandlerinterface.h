#ifndef WCPSERVERHANDLERINTERFACE_H
#define WCPSERVERHANDLERINTERFACE_H

#include "wcpserver.h"

#include <QMutex>
#include <QList>
#include <QObject>

class App;
class WCPClientInterface;
class WCPServerHandlerInterface : public QObject
{
    Q_OBJECT
protected:
    WCPServer *                     mServer             = nullptr;
    QMutex                          mActiveClientsLock;
    QList<WCPClientInterface *>     mActiveClients;

    virtual void _addActiveClient(WCPClientInterface * iClient);

public:
    explicit WCPServerHandlerInterface(QObject * iParent = nullptr) : QObject(iParent)
    { }

    virtual ~WCPServerHandlerInterface()
    { }

    virtual void setWCPServer(WCPServer * iServer);
    virtual WCPServer * getWCPServer() const        { return mServer; }

public slots:
    virtual void clientDisconnected(WCPClientInterface * iClient);
    virtual void serverStarted()                                        = 0;
    virtual void serverStopped()                                        = 0;
    virtual void serverFailedToStart()                                  = 0;
    virtual void serverFailedToStop()                                   = 0;
    virtual void newConnectionArrived()                                 = 0;
};

#endif // WCPSERVERHANDLERINTERFACE_H
