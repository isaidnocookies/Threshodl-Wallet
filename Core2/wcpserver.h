#ifndef WCPSERVER_H
#define WCPSERVER_H

#include "wcpconnection.h"

#include <QString>
#include <QMutex>
#include <QList>
#include <QHostAddress>
#include <QSslConfiguration>
#include <QObject>
#include <QWebSocketServer>

class WCPServer : public QObject
{
    Q_OBJECT
public:
    explicit WCPServer(QObject *iParent = nullptr);
    ~WCPServer();

    bool isListening() const;
    WCPConnection * nextConnection();

signals:
    void failedToStartListening();
    void startedListening();
    void failedToStopListening();
    void stoppedListening();
    void newConnection();

public slots:
    void startListening(quint16 iPort, const QString &iServerName, const QSslConfiguration &iSslConfiguration, const QHostAddress &iAddress = QHostAddress::Any);
    void startListening(const QHostAddress &iAddress, quint16 iPort, const QString &iServerName, const QSslConfiguration &iSslConfiguration);
    void stopListening();

private:
    quint16                 mPort               = 0;
    QString                 mServerName;
    QHostAddress            mHostAddress        = QHostAddress::Any;
    QSslConfiguration       mSslConfiguration;
    QWebSocketServer *      mSocketServer       = nullptr;
    QMutex                  mConnectionsLock;
    QList<WCPConnection *>  mConnections;
};

#endif // WCPSERVER_H
