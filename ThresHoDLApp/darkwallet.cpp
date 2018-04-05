#include "darkwallet.h"
#include "ui_darkwallet.h"

#include "globalsandstyle.h"

#include <QScroller>

DarkWallet::DarkWallet(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DarkWallet)
{
    ui->setupUi(this);

    ui->sendButton->setStyleSheet(darkButtonStyle());
    ui->receiveButton->setStyleSheet(darkButtonStyle());
    ui->withdrawToBrightWalletButton->setStyleSheet(darkButtonStyle());

    QScroller::grabGesture(ui->microWalletListWidget, QScroller::LeftMouseButtonGesture);

    for (int i = 0; i < 200; i++) {
        ui->microWalletListWidget->insertItem(i, new QListWidgetItem(QString("0.%1 - 456n7e8km7n6rb5v4b6n7m84m7n6b5v5765tghj").arg(i)));
    }

}

DarkWallet::~DarkWallet()
{
    delete ui;
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

}

void DarkWallet::on_withdrawToBrightWalletButton_pressed()
{

}
