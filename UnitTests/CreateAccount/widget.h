#ifndef WIDGET_H
#define WIDGET_H

#include "rpcconnection.h"
#include "bitcoininterface.h"
#include "certificate.h"
#include "encryptionkey.h"

#include <QWidget>
#include <QSslKey>
#include <QSslCertificate>
#include <QSslConfiguration>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private slots:
    void connectedToServer();
    void disconnectedFromServer();
    void failedToSendMessage();
    void sentMessage();
    void receivedMessage();
    void socketError(QAbstractSocket::SocketError iError);
    void sslErrors(const QList<QSslError> iErrors);

    void on_goButton_pressed();

private:
    Ui::Widget *ui;

    RPCConnection *     mConnection         = nullptr;
    QByteArray          mCACert;
    QSslConfiguration   mSslConfiguration;
    BitcoinWalletRef    mBitcoinWallet;
    QByteArray          mThisPublicKey;
    QByteArray          mThisPrivateKey;
};

#endif // WIDGET_H
