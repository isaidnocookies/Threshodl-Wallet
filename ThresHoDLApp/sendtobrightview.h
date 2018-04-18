#ifndef SENDTOBRIGHTVIEW_H
#define SENDTOBRIGHTVIEW_H

#include "rpcconnection.h"
#include "useraccount.h"

#include <QWidget>
#include <QSslConfiguration>
#include <QSslCertificate>
#include <QSslKey>

namespace Ui {
class SendToBrightView;
}

class SendToBrightView : public QWidget
{
    Q_OBJECT

public:
    explicit SendToBrightView(QWidget *parent = 0);
    ~SendToBrightView();

    void setActiveUser(UserAccount *iUser);

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

    void on_amountLineEdit_textChanged(const QString &arg1);

signals:
    void updateDarkBalanceOnDarkWallet();

private:
    Ui::SendToBrightView *ui;

    RPCConnection           *mConnection;
    QSslConfiguration       mSslConfiguration;
    QString                 mTransactionId;
    UserAccount             *mActiveUser;

    void startProgressBarAndDisable();
    void stopProgressBarAndEnable();
    void completeWalletsAndAdd(QMap<QString, QByteArray> iData);
    QList<BitcoinWallet> getWalletsToComplete(QStringMath iValue);
};

#endif // SENDTOBRIGHTVIEW_H
