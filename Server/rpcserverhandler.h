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
    ~RPCServerHandler();

signals:

public slots:
    void clientDisconnected();
    void serverStarted();
    void serverFailedToStart();
    void newConnectionArrived();

private:
    QList<ClientConnection *>   mActiveClientConnections;
};

#endif // RPCSERVERHANDLER_H
