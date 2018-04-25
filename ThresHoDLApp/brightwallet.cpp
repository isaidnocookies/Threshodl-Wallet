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
    mBalance = mActiveUser->getBrightBalance();
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

        // moved to sendtodarkview...
//        mActiveUser->setBrightBalance(lBrightAmount.toString());
//        for (auto w : iDarkWallets) {
//            mActiveUser->addMicroWallet(BitcoinWallet(w));
//            qDebug() << "Adding.... " << BitcoinWallet(w).walletId();
//        }

//        mActiveUser->updateBalancesForViews(mActiveUser->getBrightBalance().toString(), mActiveUser->getDarkBalance().toString());
        mActiveUser->updateBrightBalanceFromBlockchain();

        updateBrightBalance(mActiveUser->getBrightBalance());
        emit brightToDarkCompletedSignal(iSuccessful, lBrightAmount.toString(), iDarkWallets);
    } else {
        //failed
        qDebug() << "Failed to import wallets";
    }
}

void BrightWallet::updateBrightBalanceFromBlockchain(QString iWalletId, QString iValue)
{
    qDebug() << "Update from blockchain!";
    ui->totalLabel->setText(iValue);
}

void BrightWallet::updateBalancesForViews(QString iBright, QString iDark)
{
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
    QString lTotalBalance = mActiveUser->getBrightBalance().toString();

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
}
