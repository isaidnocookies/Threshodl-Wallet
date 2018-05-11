#ifndef WCPSERVERHANDLERINTERFACE_H
#define WCPSERVERHANDLERINTERFACE_H

#include "wcpserver.h"

#include <QList>
#include <QObject>

class WCPClientInterface;
class WCPServerHandlerInterface : public QObject
{
    Q_OBJECT
protected:
    QList<WCPClientInterface *> mActiveClients;

public:
    explicit WCPServerHandlerInterface(QObject * iParent = nullptr);
    virtual ~WCPServerHandlerInterface();

signals:

public slots:
    virtual void clientDisconnected(WCPClientInterface * iClient);
    virtual void serverStarted();
    virtual void serverFailedToStart();
    virtual void newConnectionArrived();
};

#endif // WCPSERVERHANDLERINTERFACE_H
