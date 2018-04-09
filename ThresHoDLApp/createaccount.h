#ifndef CREATEACCOUNT_H
#define CREATEACCOUNT_H

#include "rpcconnection.h"

#include <QWidget>
#include <QDebug>
#include <QSslConfiguration>
#include <QSslCertificate>
#include <QSslKey>

namespace Ui {
class CreateAccount;
}

class CreateAccount : public QWidget
{
    Q_OBJECT

public:
    explicit CreateAccount(QWidget *parent = nullptr);
    ~CreateAccount();

private slots:
    void on_createAccountButton_pressed();
    void connectedToServer();
    void disconnectedFromServer();
    void failedToSendMessage();
    void sentMessage();
    void receivedMessage();
    void socketError(QAbstractSocket::SocketError iError);
    void sslErrors(const QList<QSslError> iErrors);

signals:
    void createUserAccount(QString oUsername, QByteArray oPriv, QByteArray oPub);

private:
    Ui::CreateAccount   *ui;
    RPCConnection       *mConnection;
    QSslConfiguration   mSslConfiguration;

    QByteArray          mPrivateKey;
    QByteArray          mPublicKey;
    QString             mUsername;

    void createUser(QString iUsername);
    void startProgressBarAndDisable();
    void stopProgressBarAndEnable();
};

#endif // CREATEACCOUNT_H
