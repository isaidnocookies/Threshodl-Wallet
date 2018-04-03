#ifndef CLIENTCONNECTION_H
#define CLIENTCONNECTION_H

#include "rpcconnection.h"

#include <QTimer>

class RPCServerHandler;
class ClientConnection : public QObject
{
    Q_OBJECT
public:
    explicit ClientConnection(RPCConnection * iConnection, RPCServerHandler *iServer);

signals:
    void clientDisconnected();

public slots:

protected slots:
    void timerCheckBuffer();
    void processIncomingMessage();
    void outgoingMessageSent();
    void outgoingMessageFailedToSend();

private:
    QTimer *            mCheckBufferTimer;
    RPCServerHandler *  mServer;
    RPCConnection *     mConnection;

    QString username() const;

    bool                mAuthenticated  = false;
    QString             mUsername;
};

#endif // CLIENTCONNECTION_H
