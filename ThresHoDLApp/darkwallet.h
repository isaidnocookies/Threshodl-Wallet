#ifndef DARKWALLET_H
#define DARKWALLET_H

#include <QWidget>

#include "darksendview.h"

namespace Ui {
class DarkWallet;
}

class DarkWallet : public QWidget
{
    Q_OBJECT

public:
    explicit DarkWallet(QWidget *parent = nullptr);
    ~DarkWallet();

private slots:
    void on_closeWindowButton_pressed();
    void on_sendButton_pressed();
    void on_receiveButton_pressed();
    void on_withdrawToBrightWalletButton_pressed();

private:
    Ui::DarkWallet  *ui;
    DarkSendView    *mDarkSendView;
};

#endif // DARKWALLET_H
