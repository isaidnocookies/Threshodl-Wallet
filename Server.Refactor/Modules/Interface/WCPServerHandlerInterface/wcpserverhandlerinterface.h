#ifndef WCPSERVERHANDLERINTERFACE_H
#define WCPSERVERHANDLERINTERFACE_H

#include "wcpconnection.h"

#include <QObject>
#include <QTimer>

class WCPServerInterface;
class WCPServerHandlerInterface : public QObject
{
    Q_OBJECT
protected:
    WCPServerInterface *    mServer;
    WCPConnection *         mConnection;

public:
    explicit WCPServerHandlerInterface(WCPConnection * iConnection, WCPServerInterface * iServer, QObject *iParent = nullptr)
        : QObject(iParent), mConnection(iConnection)
    { }

    virtual ~WCPServerHandlerInterface()
    { }

signals:
    void clientDisconnected();

public slots:
    virtual bool sendMessage(const QString &iMessage);

protected slots:
    virtual void timerCheckBuffer();
    virtual void processIncomingMessage();
    virtual void outgoingMessageSent();
    virtual void outgoingMessageFailedToSend();
    virtual void socketError(QAbstractSocket::SocketError iError);
    virtual void sslErrors(const QList<QSslError> iErrors);
};


#endif // WCPSERVERHANDLERINTERFACE_H
