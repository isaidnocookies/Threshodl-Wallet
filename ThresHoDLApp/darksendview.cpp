#include "darksendview.h"
#include "ui_darksendview.h"
#include "globalsandstyle.h"

DarkSendView::DarkSendView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DarkSendView)
{
    ui->setupUi(this);

    ui->scanQrButton->setStyleSheet(darkButtonStyle());
    ui->sendTransactionButton->setStyleSheet(darkButtonStyle());
}

DarkSendView::~DarkSendView()
{
    delete ui;
}

void DarkSendView::on_closePushButton_pressed()
{
    this->hide();
    this->deleteLater();
}

void DarkSendView::on_scanQrButton_pressed()
{

}

void DarkSendView::on_sendTransactionButton_pressed()
{
    QString lAmount         = ui->amountLineEdit->text();
    QString lAddress        = ui->addressLineEdit->text();
    QString lEmailAddress   = ui->emailAddressLineEdit->text();

    if (lAmount.isEmpty() || lAddress.isEmpty() || lEmailAddress.isEmpty()) {
        ui->sendWarningLabel->setText("Please complete all fields!");
    } else {
        // do something..
        ui->sendWarningLabel->setText("EVERTHING IS AWESOME!");
    }
}
