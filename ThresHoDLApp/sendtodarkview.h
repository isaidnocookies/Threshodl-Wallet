#ifndef SENDTODARKVIEW_H
#define SENDTODARKVIEW_H

#include "rpcconnection.h"

#include <QWidget>
#include <QDebug>
#include <QSslConfiguration>
#include <QSslCertificate>
#include <QSslKey>

namespace Ui {
class SendToDarkView;
}

class SendToDarkView : public QWidget
{
    Q_OBJECT

public:
    explicit SendToDarkView(QWidget *parent = 0);
    ~SendToDarkView();

private slots:
    void on_closeButton_pressed();
    void on_convertButton_pressed();

    void connectedToServer();
    void disconnectedFromServer();
    void failedToSendMessage();
    void sentMessage();
    void receivedMessage();
    void socketError(QAbstractSocket::SocketError iError);
    void sslErrors(const QList<QSslError> iErrors);

    void on_confirmCheckBox_stateChanged(int arg1);

private:
    Ui::SendToDarkView *ui;
    RPCConnection       *mConnection;
    QSslConfiguration   mSslConfiguration;

    void startProgressBarAndDisable();
    void stopProgressBarAndEnable();
};

#endif // SENDTODARKVIEW_H
