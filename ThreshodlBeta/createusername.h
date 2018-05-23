#ifndef CREATEUSERNAME_H
#define CREATEUSERNAME_H

#include "core.h"

#include <QObject>
#include <QSslConfiguration>
#include <QSslCertificate>
#include <QSslKey>
#include <QDebug>

class CreateUsername : public QObject
{
    Q_OBJECT
public:
    explicit CreateUsername(QObject *parent = nullptr);
    void create(QString iUsername);

signals:
    void usernameCreated(bool oSuccess, QString oUsername, QByteArray oPublicKey, QByteArray oPrivateKey);

private slots:
    void connectedToServer();
    void disconnectedFromServer();
    void failedToSendMessage();
    void sentMessage();
    void receivedMessage();
    void socketError(QAbstractSocket::SocketError iError);
    void sslErrors(const QList<QSslError> iErrors);

private:
    WCPConnection       *mConnection;
    QSslConfiguration   mSslConfiguration;

    QByteArray          mPrivateKey;
    QByteArray          mPublicKey;
    QString             mUsername;

    QString             mTransactionId;
};

#endif // CREATEUSERNAME_H
