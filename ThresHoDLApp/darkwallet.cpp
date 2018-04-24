#include "darkwallet.h"
#include "ui_darkwallet.h"

#include "globalsandstyle.h"
#include "qrencoder.h"
#include "serializer.h"

#include <QScroller>
#include <QFont>
#include <QDesktopWidget>
#include <QCoreApplication>
#include <QDesktopServices>

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

    mMainBalanceFont = ui->balancePushButton->font();

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
    connect (mActiveUser, &UserAccount::updateBalancesForViews, this, &DarkWallet::updateBalancesForViews);
    updateBalanceLabel();
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
    updateBalanceLabel();
}

void DarkWallet::updateBalancesForViews(QString iBright, QString iDark)
{
    Q_UNUSED (iBright)
    Q_UNUSED (iDark)

    updateBalanceLabel();
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

void DarkWallet::updateBalanceLabel()
{
    QString lTotalBalance = mActiveUser->getDarkBalance().toString();

    bool    lFontFits = false;
    QFont   lFont = mMainBalanceFont;

    while (!lFontFits) {
        QFontMetrics lFm(lFont);
        QRect lBound = lFm.boundingRect(0,0, ui->balancePushButton->width(), ui->balancePushButton->height(), Qt::TextWordWrap | Qt::AlignCenter, lTotalBalance);

        if (lBound.width() <= QApplication::desktop()->screenGeometry().width() - 20)
            lFontFits = true;
        else
            lFont.setPointSize(lFont.pointSize() - 2);
    }

    ui->balancePushButton->setText(QString("%1").arg(lTotalBalance));
    ui->balancePushButton->setFont(lFont);
}
