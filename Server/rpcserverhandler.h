#ifndef RPCSERVERHANDLER_H
#define RPCSERVERHANDLER_H

#include "clientconnection.h"

#include <QList>
#include <QObject>

class RPCServerHandler : public QObject
{
    Q_OBJECT
public:
    explicit RPCServerHandler(QObject *iParent = nullptr);

signals:

public slots:
    void serverStarted();
    void serverFailedToStart();
    void newConnectionArrived();

private:
    QList< QSharedPointer<ClientConnection> >   mActiveClientConnections;
};

#endif // RPCSERVERHANDLER_H
