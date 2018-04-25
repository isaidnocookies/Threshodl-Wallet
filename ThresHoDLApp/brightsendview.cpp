#include "brightsendview.h"
#include "ui_brightsendview.h"

#include "globalsandstyle.h"

BrightSendView::BrightSendView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BrightSendView)
{
    ui->setupUi(this);

    ui->sendTransactionButton->setStyleSheet(lightBackgroundStyleSheet());
    ui->addressLineEdit->setStyleSheet(lightBackgroundStyleSheet());
    ui->amountLineEdit->setStyleSheet(lightBackgroundStyleSheet());

    ui->progressBar->setVisible(false);
}

BrightSendView::~BrightSendView()
{
    delete ui;
}

void BrightSendView::setActiveUser(UserAccount *iActiveUser)
{
    mActiveUser = iActiveUser;
    ui->availableBalanceLabel->setText(QString("(Available Balance: %1)").arg(mActiveUser->getBrightBalance().toString()));
}

void BrightSendView::on_closePushButton_pressed()
{
    this->hide();
    this->deleteLater();
}

void BrightSendView::on_scanQrPushButton_pressed()
{
    mQrReaderView = new QrReader;

    connect(mQrReaderView, &QrReader::sendCameraCode, this, &BrightSendView::getCameraCode);

    mQrReaderView->show();
    mQrReaderView->showMaximized();
}

void BrightSendView::getCameraCode(QString iCode)
{
    ui->addressLineEdit->setText(iCode);
    // possibly do some checking of the code before a send is initiated
}

bool BrightSendView::sendTransaction(QString iAddress, QString iAmount)
{
    return mActiveUser->sendBrightTransaction(iAddress, iAmount);
}

void BrightSendView::startProgressBarAndDisable()
{
    ui->progressBar->setVisible(true);
    ui->closePushButton->setEnabled(false);
    ui->sendTransactionButton->setEnabled(false);
    ui->scanQrPushButton->setEnabled(false);
    ui->amountLineEdit->setEnabled(false);
    ui->addressLineEdit->setEnabled(false);
}

void BrightSendView::stopProgressBarAndEnable()
{
    ui->progressBar->setVisible(false);
    ui->closePushButton->setEnabled(true);
    ui->sendTransactionButton->setEnabled(true);
    ui->scanQrPushButton->setEnabled(true);
    ui->amountLineEdit->setEnabled(true);
    ui->addressLineEdit->setEnabled(true);
}

void BrightSendView::on_sendTransactionButton_released()
{
    if (ui->amountLineEdit->text().isEmpty() || ui->addressLineEdit->text().isEmpty()) {
        ui->warningLabel->setText("Please complete all fields");
        return;
    } else if (!ui->checkBox->isChecked()){
        ui->warningLabel->setText("Please check the checkbox!");
        return;
    }

    startProgressBarAndDisable();

    if (sendTransaction(ui->addressLineEdit->text(), ui->amountLineEdit->text())) {
        ui->availableBalanceLabel->setText(QString("(Available Balance: %1)").arg(mActiveUser->getBrightBalance().toString()));
        ui->warningLabel->setText("Transaction Sent!");
    } else {
        ui->warningLabel->setText("Transaction Failed!");
    }

    stopProgressBarAndEnable();
}
