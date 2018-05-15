#ifndef DARKSENDVIEW_H
#define DARKSENDVIEW_H

#include "core.h"
#include "darksendconfirm.h"
#include "qrreader.h"
#include "useraccount.h"
#include "breakdownmicrowallet.h"

#include <QWidget>
#include <QDebug>
#include <QSslConfiguration>
#include <QSslCertificate>
#include <QSslKey>
#include <QJsonObject>
#include <QJsonDocument>

namespace Ui {
class DarkSendView;
}

class DarkSendView : public QWidget
{
    Q_OBJECT

public:
    explicit DarkSendView(QWidget *parent = nullptr);
    void setEmail(QString iEmail);
    void setActiveUser(UserAccount *iActiveUser);
    ~DarkSendView();

private slots:
    void on_closePushButton_pressed();
    void on_scanQrButton_pressed();
    void on_sendTransactionButton_pressed();
    void on_amountLineEdit_textChanged(const QString &arg1);
    void on_addressLineEdit_textChanged(const QString &arg1);
    void on_emailAddressLineEdit_textChanged(const QString &arg1);

    void connectedToServer();
    void disconnectedFromServer();
    void failedToSendMessage();
    void sentMessage();
    void receivedMessage();
    void socketError(QAbstractSocket::SocketError iError);
    void sslErrors(const QList<QSslError> iErrors);

    void sentConfirmation(bool iSuccess);

public slots:
    void getQrCode(QString iData);
    void newMicroWallets(bool iSuccess);
    void completeMicroWallet(bool oSuccess);
    void microWalletBreakdownComplete(bool oSuccess);

signals:
    void updateBalance();

private:
    Ui::DarkSendView        *ui;
    WCPConnection           *mConnection;
    QSslConfiguration       mSslConfiguration;
    DarkSendConfirm         *mDarkSendConfirmationView;
    QrReader                *mQrReaderView;
    UserAccount             *mActiveUser;
    QList<BitcoinWallet>    mWalletsToSend_Pending;
    QStringMath             mPendingAmountToSend;
    BreakdownMicroWallet    *mBreakdownMicroWallet;

    QString                 mTransactionID;
    int                     mBreakdownAttempts;

    void stopProgressBarAndEnable();
    void startProgressBarAndDisable();

    bool getAttachmentPackage(QByteArray &oData);
};

#endif // DARKSENDVIEW_H
