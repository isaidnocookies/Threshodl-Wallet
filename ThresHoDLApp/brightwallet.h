#ifndef BRIGHTWALLET_H
#define BRIGHTWALLET_H

#include "brightsendview.h"
#include "sendtodarkview.h"
#include "qrencoder.h"

#include <QWidget>
#include <QImage>
#include <QApplication>
#include <QMainWindow>

#include "useraccount.h"

namespace Ui {
class BrightWallet;
}

class BrightWallet : public QWidget
{
    Q_OBJECT

public:
    explicit BrightWallet(QWidget *parent = 0);
    ~BrightWallet();

    void setAddress(QByteArray iPublicAddress);
    void setBalance(double iAmount);
    void setActiveUser(UserAccount &iUserAccount);

public slots:
    void updateBrightBalance (double lAmount);
    void brightToDarkCompleted(bool iSuccessful, double lBrightAmount, QList<QByteArray> iDarkWallets);

private slots:
    void on_closeWindowButton_pressed();
    void on_sendButton_pressed();
    void on_sendToDarkWalletButton_pressed();

signals:
    void makeDashboardMaximized();
    void updateDarkBalance(double lAmount);
    void updateBrightBalanceSignal(double lAmount);
    void brightToDarkCompletedSignal(bool iSuccessful, double lBrightAmount, QList<QByteArray> iDarkWallets);

private:
    Ui::BrightWallet    *ui;
    BrightSendView      *mBrightSendView;
    SendToDarkView      *mSendToDarkView;
    QImage              *mQrImage;
    QByteArray          mPublicAddress;
    double              mBalance;
    UserAccount         *mActiveUser;

    void setQrCode();
};

#endif // BRIGHTWALLET_H
