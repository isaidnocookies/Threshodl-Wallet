#include "darkmicrowalletview.h"
#include "ui_darkmicrowalletview.h"

DarkMicroWalletView::DarkMicroWalletView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DarkMicroWalletView)
{
    ui->setupUi(this);
}

DarkMicroWalletView::~DarkMicroWalletView()
{
    delete ui;
}

void DarkMicroWalletView::on_closeWindowButton_pressed()
{
    this->hide();
    this->deleteLater();
}
