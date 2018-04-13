#include "brightwallet.h"
#include "ui_brightwallet.h"

#include "globalsandstyle.h"

BrightWallet::BrightWallet(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BrightWallet)
{
    ui->setupUi(this);

    ui->sendButton->setStyleSheet(lightBackgroundStyleSheet());
    ui->sendToDarkWalletButton->setStyleSheet(lightBackgroundStyleSheet());
}

BrightWallet::~BrightWallet()
{
    delete ui;
}

void BrightWallet::setAddress(QByteArray iPublicAddress)
{
    mPublicAddress = iPublicAddress;
    setQrCode();
}

void BrightWallet::setBalance(double iAmount)
{
    mBalance = iAmount;
}

void BrightWallet::setActiveUser(UserAccount &iUserAccount)
{
    mActiveUser = &iUserAccount;

    ui->totalLabel->setText(QString("%1").arg(mActiveUser->getBrightBalance()));
}

void BrightWallet::updateBrightBalance(double lAmount)
{
    double newBalance = mBalance - lAmount;
    ui->totalLabel->setText(QString::number(newBalance));
    mBalance = newBalance;

    emit updateBrightBalanceSignal(lAmount);
}

void BrightWallet::on_closeWindowButton_pressed()
{
    this->hide();
    this->deleteLater();
}

void BrightWallet::on_sendButton_pressed()
{
    mBrightSendView = new BrightSendView;

    mBrightSendView->show();
    mBrightSendView->showMaximized();
}

void BrightWallet::on_sendToDarkWalletButton_pressed()
{
    mSendToDarkView = new SendToDarkView;

    connect (mSendToDarkView, &SendToDarkView::updateBrightBalance, this, &BrightWallet::updateBrightBalance);
    connect (mSendToDarkView, &SendToDarkView::brightToDarkCompleted, this, &BrightWallet::brightToDarkCompleted);

    mSendToDarkView->setBalance(mActiveUser->getBrightBalance());
    mSendToDarkView->setValues(mActiveUser->getPrivateKey(), mActiveUser->getUsername());

    mSendToDarkView->show();
    mSendToDarkView->showMaximized();
}

void BrightWallet::setQrCode()
{
    mQrImage = new QImage();
    *mQrImage = QrEncoder::createQrCode(mPublicAddress);
    ui->qrCodeLabel->setPixmap(QPixmap::fromImage(*mQrImage));
    ui->qrCodeLabel->setFixedSize(ui->qrAddressLabel->width() * 2, ui->qrAddressLabel->width() * 2);
}
