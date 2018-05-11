#ifndef WCPCLIENTINTERFACE_H
#define WCPCLIENTINTERFACE_H

#include "wcpconnection.h"
#include "wcpmessage.h"

#include <QObject>
#include <QTimer>

class WCPServerHandlerInterface;
class WCPClientInterface : public QObject
{
    Q_OBJECT
protected:
    WCPServerHandlerInterface *     mServer;
    WCPConnection *                 mConnection;
    QTimer *                        mCheckBufferTimer;

public:
    explicit WCPClientInterface(WCPConnection * iConnection, WCPServerHandlerInterface * iServer, QObject *iParent = nullptr);

    virtual ~WCPClientInterface()
    { }

    virtual void processMessage(const QString iWCPVersion, const QString iCommand, const WCPMessage &iMessage)     = 0;

signals:
    void clientDisconnected(WCPClientInterface *iClient);
    void messageSent();

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


#endif // WCPCLIENTINTERFACE_H
