#ifndef RPCSERVERHANDLER_H
#define RPCSERVERHANDLER_H

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
};

#endif // RPCSERVERHANDLER_H
