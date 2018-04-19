#include "darkreceiveview.h"
#include "ui_darkreceiveview.h"

#include "globalsandstyle.h"
#include "qrencoder.h"

DarkReceiveView::DarkReceiveView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DarkReceiveView)
{
    ui->setupUi(this);

    ui->updateQRCodePushButton->setStyleSheet(darkBackgroundStyleSheet());

    ui->addressLineEdit->setStyleSheet(darkBackgroundStyleSheet());
    ui->emailLineEdit->setStyleSheet(darkBackgroundStyleSheet());

    if (mEmailAddress.isEmpty() || mThreshodlAddress.isEmpty()) {
        ui->qrAddressLabel->setText("Please complete fields!");
        ui->qrEmailAddressLabel->setText("");
    }
}

DarkReceiveView::~DarkReceiveView()
{
    delete ui;
}

void DarkReceiveView::setAddresses(QString iEmail, QString iAddress)
{
    mEmailAddress = iEmail;
    mThreshodlAddress = iAddress;

    if (!mEmailAddress.isEmpty()) {
        ui->emailLineEdit->setText(mEmailAddress);
    }
    if (!mThreshodlAddress.isEmpty()) {
        ui->addressLineEdit->setText(mThreshodlAddress);
        ui->addressLineEdit->setEnabled(false);
    }
}

void DarkReceiveView::on_closeWindowButton_pressed()
{
    this->hide();
    this->deleteLater();
}

void DarkReceiveView::on_updateQRCodePushButton_pressed()
{
    if (ui->emailLineEdit->text().isEmpty() || ui->addressLineEdit->text().isEmpty()) {
        ui->warningLabel->setText("*Please complete all the fields");
    } else {
        mEmailAddress = ui->emailLineEdit->text();
        mThreshodlAddress = ui->addressLineEdit->text();
        emit saveAddresses(mEmailAddress, mThreshodlAddress);
        on_closeWindowButton_pressed();
    }
}
