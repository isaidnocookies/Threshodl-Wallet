#include "brightwallet.h"
#include "ui_brightwallet.h"

#include "globalsandstyle.h"

BrightWallet::BrightWallet(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BrightWallet)
{
    ui->setupUi(this);

    mQrImage = new QImage();
    *mQrImage = QrEncoder::createQrCode("THRESHODL WALLET IS AWESOME");

    ui->sendButton->setStyleSheet(lightBackgroundStyleSheet());
    ui->sendToDarkWalletButton->setStyleSheet(lightBackgroundStyleSheet());

    ui->qrCodeLabel->setPixmap(QPixmap::fromImage(*mQrImage));

    ui->qrCodeLabel->setFixedSize(ui->qrAddressLabel->width() * 2, ui->qrAddressLabel->width() * 2);
}

BrightWallet::~BrightWallet()
{
    delete ui;
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

    mSendToDarkView->show();
    mSendToDarkView->showMaximized();
}
