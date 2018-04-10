#include "darksendconfirm.h"
#include "ui_darksendconfirm.h"

#include "globalsandstyle.h"

DarkSendConfirm::DarkSendConfirm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DarkSendConfirm)
{
    ui->setupUi(this);

    ui->cancelPushButton->setStyleSheet(darkBackgroundStyleSheet());
    ui->confirmSendPushButton->setStyleSheet(darkBackgroundStyleSheet());

    ui->warningLabel->setText("");
}

DarkSendConfirm::~DarkSendConfirm()
{
    delete ui;
}

void DarkSendConfirm::on_closeWindowButton_pressed()
{
    emit sendConfirmation(false);
    this->hide();
    this->deleteLater();
}

void DarkSendConfirm::on_cancelPushButton_pressed()
{
    emit sendConfirmation(false);
    this->hide();
    this->deleteLater();
}

void DarkSendConfirm::on_confirmSendPushButton_pressed()
{
    if (ui->confirmCheckBox->isChecked()) {
        emit sendConfirmation(true);
        this->hide();
        this->deleteLater();
    } else {
        ui->warningLabel->setText("Please confirm with checkbox!");
    }
}
