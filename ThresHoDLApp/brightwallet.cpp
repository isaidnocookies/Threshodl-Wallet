#include "brightwallet.h"
#include "ui_brightwallet.h"

#include "globalsandstyle.h"

BrightWallet::BrightWallet(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BrightWallet)
{
    ui->setupUi(this);
    mBalance = QStringMath();

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

void BrightWallet::setBalance(QStringMath iAmount)
{
    mBalance = iAmount;
}

void BrightWallet::setActiveUser(UserAccount &iUserAccount)
{
    mActiveUser = &iUserAccount;

    ui->totalLabel->setText(QString("%1").arg(mActiveUser->getBrightBalance().toString()));
}

void BrightWallet::updateBrightBalance(QStringMath lAmount)
{
    ui->totalLabel->setText(lAmount.toString());
    mBalance = lAmount;
}

void BrightWallet::brightToDarkCompleted(bool iSuccessful, QStringMath lBrightAmount, QList<QByteArray> iDarkWallets)
{
    if (iSuccessful) {
        mActiveUser->setBrightBalance(lBrightAmount.toString());
        for (auto w : iDarkWallets) {
            mActiveUser->addMicroWallet(BitcoinWallet(w));
            qDebug() << "Adding.... " << BitcoinWallet(w).walletId();
        }
        updateBrightBalance(mActiveUser->getBrightBalance());
        mActiveUser->updateBalancesForMainWindow(mActiveUser->getBrightBalance().toString(), mActiveUser->getDarkBalance().toString());

        emit brightToDarkCompletedSignal(iSuccessful, lBrightAmount.toString(), iDarkWallets);
    } else {
        //failed
        qDebug() << "Failed to import wallets";
    }
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

    mSendToDarkView->setActiveUser(mActiveUser);

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
