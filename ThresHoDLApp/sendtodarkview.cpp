#include "sendtodarkview.h"
#include "ui_sendtodarkview.h"

#include "globalsandstyle.h"

SendToDarkView::SendToDarkView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SendToDarkView)
{
    ui->setupUi(this);

    ui->convertButton->setStyleSheet(lightBackgroundStyleSheet());
    ui->amountLineEdit->setStyleSheet(lightBackgroundStyleSheet());
//    ui->confirmCheckBox->setStyleSheet(lightBackgroundStyleSheet());
}

SendToDarkView::~SendToDarkView()
{
    delete ui;
}

void SendToDarkView::on_closeButton_pressed()
{
    this->hide();
    this->deleteLater();
}

void SendToDarkView::on_convertButton_pressed()
{
    if (ui->confirmCheckBox->isChecked()) {
        // Allow transfer
    } else {
        // Don't allow
        ui->warningLabelForCheck->setText("*REQUIRED");
    }
}
