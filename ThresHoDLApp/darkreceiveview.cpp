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
        ui->qrCodeLabel->setText("?");
        ui->qrAddressLabel->setText("Please complete fields!");
        ui->qrEmailAddressLabel->setText("");
    } else {
        createQrCode();
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
    }

    if (!mEmailAddress.isEmpty() && !mThreshodlAddress.isEmpty()) {
        createQrCode();
    }
}

void DarkReceiveView::createQrCode()
{
    mQrImage = new QImage();
    *mQrImage = QrEncoder::createQrCode(QString("%1 - %2").arg(mThreshodlAddress).arg(mEmailAddress));
    ui->qrCodeLabel->setPixmap(QPixmap::fromImage(*mQrImage));
    ui->qrCodeLabel->setFixedSize(ui->qrCodeLabel->width(), ui->qrCodeLabel->width());
    ui->qrCodeLabel->setText("");
    ui->qrAddressLabel->setText(mThreshodlAddress);
    ui->qrEmailAddressLabel->setText(mEmailAddress);
}

void DarkReceiveView::on_closeWindowButton_pressed()
{
    this->hide();
    this->deleteLater();
}

void DarkReceiveView::on_updateQRCodePushButton_pressed()
{
    if (ui->emailLineEdit->text().isEmpty() || ui->addressLineEdit->text().isEmpty()) {
        ui->qrCodeLabel->setText("Please complete fields");
    } else {
        mEmailAddress = ui->emailLineEdit->text();
        mThreshodlAddress = ui->addressLineEdit->text();
        createQrCode();
        emit saveAddresses(mEmailAddress, mThreshodlAddress);
    }
}
