#ifndef SENDTODARKVIEW_H
#define SENDTODARKVIEW_H

#include "rpcconnection.h"
#include "bitcoinwallet.h"

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
    explicit SendToDarkView(QWidget *parent = nullptr);
    ~SendToDarkView();

    void setBalance(double iBalance);
    void setValues(QByteArray iPriv, QString iUsername);

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

signals:
    void addMicroWalletsToAccount (QList<BitcoinWallet> oWallets);
    void updateBrightBalance (double iAmountToDeduct);
    void brightToDarkCompleted(bool iSuccessful, double lBrightAmount, QList<BitcoinWallet> iDarkWallets);

private:
    Ui::SendToDarkView *ui;
    RPCConnection       *mConnection;
    QSslConfiguration   mSslConfiguration;

    double              mBalance;
    QByteArray          mPrivateKey;
    QString             mUsername;

    void startProgressBarAndDisable();
    void stopProgressBarAndEnable();

    void parseBitcoinPackage (QByteArray iData);
};

#endif // SENDTODARKVIEW_H
