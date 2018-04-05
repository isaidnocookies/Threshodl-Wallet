#ifndef BRIGHTWALLET_H
#define BRIGHTWALLET_H

#include "brightsendview.h"
#include "sendtodarkview.h"
#include "qrencoder.h"

#include <QWidget>
#include <QImage>
#include <QApplication>
#include <QMainWindow>

namespace Ui {
class BrightWallet;
}

class BrightWallet : public QWidget
{
    Q_OBJECT

public:
    explicit BrightWallet(QWidget *parent = 0);
    ~BrightWallet();

private slots:
    void on_closeWindowButton_pressed();
    void on_sendButton_pressed();
    void on_sendToDarkWalletButton_pressed();

signals:
    void makeDashboardMaximized();

private:
    Ui::BrightWallet    *ui;
    BrightSendView      *mBrightSendView;
    SendToDarkView      *mSendToDarkView;
    QImage              *mQrImage;
};

#endif // BRIGHTWALLET_H
