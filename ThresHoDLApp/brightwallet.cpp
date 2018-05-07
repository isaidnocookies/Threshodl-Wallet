#include "brightwallet.h"
#include "ui_brightwallet.h"

#include "globalsandstyle.h"

#include <QDesktopWidget>
#include <QCoreApplication>
#include <QDesktopServices>

BrightWallet::BrightWallet(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BrightWallet)
{
    ui->setupUi(this);
    mBalance = QStringMath();

    ui->progressBar->setVisible(false);

    ui->sendButton->setStyleSheet(lightBackgroundStyleSheet());
    ui->sendToDarkWalletButton->setStyleSheet(lightBackgroundStyleSheet());
    mMainBalanceFont = ui->totalLabel->font();
}

BrightWallet::~BrightWallet()
{
    delete ui;
}

void BrightWallet::setActiveUser(UserAccount &iUserAccount)
{
    mActiveUser = &iUserAccount;
    mBalance = mActiveUser->getBrightPendingBalance();
    mPublicAddress = mActiveUser->getBrightWallet().address();

    connect (mActiveUser, &UserAccount::updateBalancesForViews, this, &BrightWallet::updateBalancesForViews);

    updateBalanceLabel();
    setQrCode();
}

void BrightWallet::updateBrightBalance(QStringMath lAmount)
{
    ui->totalLabel->setText(lAmount.toString());
    mBalance = lAmount;
}

void BrightWallet::brightToDarkCompleted(bool iSuccessful, QStringMath lBrightAmount, QList<QByteArray> iDarkWallets)
{
    if (iSuccessful) {
        mActiveUser->updateBalancesFromBlockchain();

        updateBrightBalance(mActiveUser->getBrightPendingBalance());
        emit brightToDarkCompletedSignal(iSuccessful, lBrightAmount.toString(), iDarkWallets); // essentially updates balance in main...
    } else {
        qDebug() << "Failed to import wallets";
        emit brightToDarkCompletedSignal(false, lBrightAmount.toString(), QList<QByteArray>());
    }
}

void BrightWallet::updateBrightBalanceFromBlockchain(QString iWalletId, QString iValue)
{
    Q_UNUSED(iWalletId)
    Q_UNUSED(iValue)

    qDebug() << "Update from blockchain!";
    ui->totalLabel->setText(iValue);
}

void BrightWallet::updateBalancesForViews(QString iBright, QString iDark)
{
    Q_UNUSED(iBright)
    Q_UNUSED(iDark)

    updateBalanceLabel();
}

void BrightWallet::on_closeWindowButton_pressed()
{
    this->hide();
    this->deleteLater();
}

void BrightWallet::on_sendButton_pressed()
{
    mBrightSendView = new BrightSendView;
    mBrightSendView->setActiveUser(mActiveUser);

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
    ui->qrAddressLabel->setText(QString("Address: %1").arg(QString(mPublicAddress)));
    ui->qrCodeLabel->setFixedSize(ui->qrAddressLabel->width() * 2, ui->qrAddressLabel->width() * 2);
}

void BrightWallet::updateBalanceLabel()
{
    QStringMath lBalance = mActiveUser->getBrightBalance();
    QStringMath lPendingBalance = mActiveUser->getBrightPendingBalance();

    QString lTotalBalance = lPendingBalance.toString();

    bool    lFontFits = false;
    QFont   lFont = mMainBalanceFont;

    while (!lFontFits) {
        QFontMetrics lFm(lFont);
        QRect lBound = lFm.boundingRect(0,0, ui->totalLabel->width(), ui->totalLabel->height(), Qt::TextWordWrap | Qt::AlignCenter, lTotalBalance);

        if (lBound.width() <= QApplication::desktop()->screenGeometry().width() - 20)
            lFontFits = true;
        else
            lFont.setPointSize(lFont.pointSize() - 2);
    }

    ui->totalLabel->setText(QString("%1").arg(lTotalBalance));
    ui->totalLabel->setFont(lFont);

    if (mActiveUser->isBrightWalletSettled()) {
        ui->btcLabel->setText("BTC (Confirmed)");
    } else {
        ui->btcLabel->setText("BTC (Unconfirmed)");
    }
    stopProgressBarAndEnable();
}

void BrightWallet::startProgressBarAndDisable()
{
    ui->progressBar->setVisible(true);
    ui->refreshButton->setEnabled(false);
    ui->closeWindowButton->setEnabled(false);
    ui->sendButton->setEnabled(false);
    ui->sendToDarkWalletButton->setEnabled(false);
}

void BrightWallet::stopProgressBarAndEnable()
{
    ui->progressBar->setVisible(false);
    ui->refreshButton->setEnabled(true);
    ui->closeWindowButton->setEnabled(true);
    ui->sendButton->setEnabled(true);
    ui->sendToDarkWalletButton->setEnabled(true);
}

void BrightWallet::on_refreshButton_released()
{
    startProgressBarAndDisable();
    mActiveUser->updateBalancesFromBlockchain();
}
