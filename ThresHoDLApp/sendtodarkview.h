#ifndef SENDTODARKVIEW_H
#define SENDTODARKVIEW_H

#include "rpcconnection.h"
#include "bitcoinwallet.h"
#include "qstringmath.h"
#include "useraccount.h"

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

    void setActiveUser(UserAccount *iActiveUser);

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
    void updateBrightBalance (QString iAmountToDeduct);
    void brightToDarkCompleted(bool iSuccessful, QString lBrightAmount, QList<QByteArray> iDarkWallets);

private:
    Ui::SendToDarkView *ui;
    RPCConnection       *mConnection;
    QSslConfiguration   mSslConfiguration;

    UserAccount         *mActiveUser;
    QStringMath         mBalance;
    QByteArray          mPrivateKey;
    QString             mUsername;
    QString             mTransactionId;

    void startProgressBarAndDisable();
    void stopProgressBarAndEnable();

    void completeToDarkTransaction (QList<QByteArray> iData, QString iFeeAmount);
};

#endif // SENDTODARKVIEW_H
