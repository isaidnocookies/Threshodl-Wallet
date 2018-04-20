#include "darkwallet.h"
#include "ui_darkwallet.h"

#include "globalsandstyle.h"
#include "qrencoder.h"
#include "serializer.h"

#include <QScroller>
#include <QFont>

DarkWallet::DarkWallet(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DarkWallet)
{
    ui->setupUi(this);

    ui->sendButton->setStyleSheet(darkBackgroundStyleSheet());
    ui->withdrawToBrightWalletButton->setStyleSheet(darkBackgroundStyleSheet());

    ui->progressBar->setVisible(false);

    ui->qrPushButton->setStyleSheet(QString("background-color:white; border-radius:10px; margin:5px"));
    ui->qrPushButton->setFixedSize(ui->qrPushButton->size().width(),ui->qrPushButton->size().height());

    checkToCreateQr();
}

void DarkWallet::setEmail(QString iEmail)
{
    mEmailAddress = iEmail;
    checkToCreateQr();
}

void DarkWallet::setAddress(QString iAddress)
{
    mThreshodlAddress = iAddress;
    checkToCreateQr();
}

void DarkWallet::setActiveUser(UserAccount &iActiveUser)
{
    mActiveUser = &iActiveUser;
    ui->balancePushButton->setText(QString("%1").arg(mActiveUser->getDarkBalance().toString()));
    ui->addressLabel->setText(mActiveUser->getUsername());
}

DarkWallet::~DarkWallet()
{
    delete ui;
}

void DarkWallet::saveAddresses(QString iEmail, QString iAddress)
{
    mEmailAddress = iEmail;
    mThreshodlAddress = iAddress;

    emit saveAddressSettings(iEmail, iAddress);
    checkToCreateQr();
}

void DarkWallet::updateBalance()
{
    ui->balancePushButton->setText(mActiveUser->getDarkBalance().toString());
}

void DarkWallet::createQrCode()
{
    mQrImage = new QImage();
    QByteArray lQrCodeData = Serializer::serializeDarkQrCode(mThreshodlAddress, mEmailAddress);
    *mQrImage = QrEncoder::createQrCode(QString(lQrCodeData));

    ui->qrPushButton->setIcon(QIcon(QPixmap::fromImage(*mQrImage)));
    ui->addressLabel->setText(mThreshodlAddress);
    ui->emailLabel->setText(mEmailAddress);

    ui->qrPushButton->setText("");
    ui->qrPushButton->setIconSize(QSize(ui->qrPushButton->size().width() - 10, ui->qrPushButton->size().width() - 10));
    ui->qrPushButton->setStyleSheet(QString("background-color:white; border-radius:10px;"));
}

void DarkWallet::checkToCreateQr()
{
    if (!mEmailAddress.isEmpty() && !mThreshodlAddress.isEmpty()) {
        createQrCode();
        ui->emailLabel->setText(mEmailAddress);
        ui->addressLabel->setText(mThreshodlAddress);
    } else {
        ui->qrPushButton->setText("(Click to Update)");
    }
}

void DarkWallet::startProgressBarAndDisable()
{
    ui->progressBar->setVisible(true);
    ui->sendButton->setEnabled(false);
    ui->withdrawToBrightWalletButton->setEnabled(false);
    ui->balancePushButton->setEnabled(false);
    ui->closeWindowButton->setEnabled(false);
    ui->qrPushButton->setEnabled(false);
}

void DarkWallet::stopProgressBarAndEnable()
{
    ui->progressBar->setVisible(false);
    ui->sendButton->setEnabled(true);
    ui->withdrawToBrightWalletButton->setEnabled(true);
    ui->balancePushButton->setEnabled(true);
    ui->closeWindowButton->setEnabled(true);
    ui->qrPushButton->setEnabled(true);
}

void DarkWallet::on_closeWindowButton_pressed()
{
    this->hide();
    this->deleteLater();
}

void DarkWallet::on_sendButton_pressed()
{
    mDarkSendView = new DarkSendView;

    mDarkSendView->setActiveUser(mActiveUser);
    connect(mDarkSendView, &DarkSendView::updateBalance, this, &DarkWallet::updateBalance);

    mDarkSendView->show();
    mDarkSendView->showMaximized();
}

void DarkWallet::on_withdrawToBrightWalletButton_pressed()
{
    mSendToDarkView = new SendToBrightView;

    mSendToDarkView->setActiveUser(mActiveUser);
    connect (mSendToDarkView, &SendToBrightView::updateDarkBalanceOnDarkWallet, this, &DarkWallet::updateBalance);

    mSendToDarkView->show();
    mSendToDarkView->showMaximized();
}

void DarkWallet::on_qrPushButton_pressed()
{
    mDarkReceiveView = new DarkReceiveView;

    connect (mDarkReceiveView, &DarkReceiveView::saveAddresses, this, &DarkWallet::saveAddresses);
    mDarkReceiveView->setAddresses(mEmailAddress,mThreshodlAddress);

    mDarkReceiveView->show();
    mDarkReceiveView->showMaximized();
}

void DarkWallet::on_balancePushButton_pressed()
{
    mDarkMicroWalletView = new DarkMicroWalletView;
    mDarkMicroWalletView->setMicroWallets(mActiveUser->getDarkWallets());

    mDarkMicroWalletView->show();
    mDarkMicroWalletView->showMaximized();
}

void DarkWallet::on_refreshWalletButton_pressed()
{
//    startProgressBarAndDisable();
}
