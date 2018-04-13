#ifndef DARKWALLET_H
#define DARKWALLET_H

#include <QWidget>
#include <QMap>

#include "darksendview.h"
#include "sendtobrightview.h"
#include "darkreceiveview.h"
#include "darkmicrowalletview.h"
#include "useraccount.h"

namespace Ui {
class DarkWallet;
}

class DarkWallet : public QWidget
{
    Q_OBJECT

public:
    explicit DarkWallet(QWidget *parent = nullptr);
    void setEmail(QString iEmail);
    void setAddress(QString iAddress);

    void setActiveUser(UserAccount &iActiveUser);
    ~DarkWallet();

public slots:
    void saveAddresses(QString iEmail, QString iAddress);

private:
    void createQrCode();
    void checkToCreateQr();

private slots:
    void on_closeWindowButton_pressed();
    void on_sendButton_pressed();
    void on_withdrawToBrightWalletButton_pressed();
    void on_qrPushButton_pressed();

    void on_balancePushButton_pressed();

signals:
    void saveAddressSettings(QString oEmail, QString oAddress);

private:
    Ui::DarkWallet          *ui;
    DarkSendView            *mDarkSendView;
    SendToBrightView        *mSendToDarkView;
    DarkReceiveView         *mDarkReceiveView;
    DarkMicroWalletView     *mDarkMicroWalletView;
    QImage                  *mQrImage;

    QString                 mEmailAddress;
    QString                 mThreshodlAddress;

    UserAccount             *mActiveUser;
};

#endif // DARKWALLET_H
