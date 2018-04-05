#include "sendtobrightview.h"
#include "ui_sendtobrightview.h"

SendToBrightView::SendToBrightView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SendToBrightView)
{
    ui->setupUi(this);
}

SendToBrightView::~SendToBrightView()
{
    delete ui;
}

void SendToBrightView::on_closeButton_pressed()
{
    this->hide();
    this->deleteLater();
}

void SendToBrightView::on_convertButton_pressed()
{
    if (!ui->confirmCheckBox->isChecked()) {
        ui->warningLabelForCheck->setText(QString("*Confirm to continue"));
    }
}
