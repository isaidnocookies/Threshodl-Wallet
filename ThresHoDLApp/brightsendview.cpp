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
}

BrightSendView::~BrightSendView()
{
    delete ui;
}

void BrightSendView::on_closePushButton_pressed()
{
    this->hide();
    this->deleteLater();
}
