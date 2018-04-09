#include "darkwallet.h"
#include "ui_darkwallet.h"

#include "globalsandstyle.h"

#include <QScroller>
#include <QFont>

DarkWallet::DarkWallet(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DarkWallet)
{
    ui->setupUi(this);

    ui->sendButton->setStyleSheet(darkBackgroundStyleSheet());
    ui->receiveButton->setStyleSheet(darkBackgroundStyleSheet());
    ui->withdrawToBrightWalletButton->setStyleSheet(darkBackgroundStyleSheet());

    QFont lNormalFont = ui->microWalletListWidget->font();
    QFont lBiggerFont = lNormalFont;
    lBiggerFont.setBold(true);
    lBiggerFont.setPointSize(20);

    QScroller::grabGesture(ui->microWalletListWidget, QScroller::LeftMouseButtonGesture);

    for (int i = 0; i < 200; i++) {
        QString lRow = QString("0.%1\nmj55b5mdzD5jQaUFf9tY8E5kxkJ5Z7CJ21").arg(i);
        ui->microWalletListWidget->insertItem(i, new QListWidgetItem());
        ui->microWalletListWidget->item(i)->setTextAlignment(Qt::AlignHCenter);
    }
}

void DarkWallet::setEmail(QString iEmail)
{
    mEmailAddress = iEmail;
}

void DarkWallet::setAddress(QString iAddress)
{
    mThreshodlAddress = iAddress;
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
}

void DarkWallet::on_closeWindowButton_pressed()
{
    this->hide();
    this->deleteLater();
}

void DarkWallet::on_sendButton_pressed()
{
    mDarkSendView = new DarkSendView;

    mDarkSendView->show();
    mDarkSendView->showMaximized();
}

void DarkWallet::on_receiveButton_pressed()
{
    mDarkReceiveView = new DarkReceiveView;

    connect (mDarkReceiveView, &DarkReceiveView::saveAddresses, this, &DarkWallet::saveAddresses);
    mDarkReceiveView->setAddresses(mEmailAddress,mThreshodlAddress);

    mDarkReceiveView->show();
    mDarkReceiveView->showMaximized();
}

void DarkWallet::on_withdrawToBrightWalletButton_pressed()
{
    mSendToDarkView = new SendToBrightView;

    mSendToDarkView->show();
    mSendToDarkView->showMaximized();
}
