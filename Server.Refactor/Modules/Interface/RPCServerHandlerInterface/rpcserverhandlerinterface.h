#ifndef RPCSERVERHANDLERINTERFACE_H
#define RPCSERVERHANDLERINTERFACE_H

#include <QObject>

class RPCServerHandlerInterface : public QObject
{
    Q_OBJECT
public:
    explicit RPCServerHandlerInterface(QObject *iParent = nullptr)
        : QObject(iParent)
    { }

    virtual ~RPCServerHandlerInterface()
    { }

signals:

public slots:
    virtual void clientDisconnected()       = 0;
    virtual void serverStarted()            = 0;
    virtual void serverFailedToStart()      = 0;
    virtual void newConnectionArrived()     = 0;
};


#endif // RPCSERVERHANDLERINTERFACE_H
