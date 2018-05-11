#ifndef WCPSERVERHANDLERINTERFACE_H
#define WCPSERVERHANDLERINTERFACE_H

#include "wcpserver.h"

#include <QList>
#include <QObject>

class App;
class WCPClientInterface;
class WCPServerHandlerInterface : public QObject
{
    Q_OBJECT
protected:
    WCPServer *                     mServer             = nullptr;
    QList<WCPClientInterface *>     mActiveClients;

public:
    explicit WCPServerHandlerInterface(QObject * iParent = nullptr);
    virtual ~WCPServerHandlerInterface();

    virtual void setWCPServer(WCPServer * iServer);
    virtual WCPServer * getWCPServer() const        { return mServer; }

signals:

public slots:
    virtual void clientDisconnected(WCPClientInterface * iClient);
    virtual void serverStarted()                                        = 0;
    virtual void serverStopped()                                        = 0;
    virtual void serverFailedToStart()                                  = 0;
    virtual void serverFailedToStop()                                   = 0;
    virtual void newConnectionArrived();
};

#endif // WCPSERVERHANDLERINTERFACE_H
