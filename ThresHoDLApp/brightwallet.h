#ifndef BRIGHTWALLET_H
#define BRIGHTWALLET_H

#include "brightsendview.h"
#include "sendtodarkview.h"
#include "qrencoder.h"

#include <QWidget>
#include <QImage>

#include "useraccount.h"
#include "bitcoinblockchaininterface.h"

namespace Ui {
class BrightWallet;
}

class BrightWallet : public QWidget
{
    Q_OBJECT

public:
    explicit BrightWallet(QWidget *parent = 0);
    ~BrightWallet();

    void setActiveUser(UserAccount &iUserAccount);

public slots:
    void updateBrightBalance (QStringMath lAmount);
    void brightToDarkCompleted(bool iSuccessful, QStringMath lBrightAmount, QList<QByteArray> iDarkWallets);
    void updateBrightBalanceFromBlockchain(QString iWalletId, QString iValue);
    void updateBalancesForViews(QString iBright, QString iDark);

private slots:
    void on_closeWindowButton_pressed();
    void on_sendButton_pressed();
    void on_sendToDarkWalletButton_pressed();

signals:
    void makeDashboardMaximized();
    void updateDarkBalance(QString lAmount);
    void updateBrightBalanceSignal(QString lAmount);
    void brightToDarkCompletedSignal(bool iSuccessful, QString lBrightAmount, QList<QByteArray> iDarkWallets);

private:
    Ui::BrightWallet            *ui;
    BrightSendView              *mBrightSendView;
    SendToDarkView              *mSendToDarkView;
    QImage                      *mQrImage;
    QByteArray                  mPublicAddress;
    QStringMath                 mBalance;
    UserAccount                 *mActiveUser;
    BitcoinBlockchainInterface  *mBlockchainInterface;
    QFont                       mMainBalanceFont;

    void setQrCode();
    void updateBalanceLabel();
};

#endif // BRIGHTWALLET_H
