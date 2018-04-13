#include "darkwalletimportview.h"
#include "ui_darkwalletimportview.h"

#include "globalsandstyle.h"

#include <QDate>

DarkWalletImportView::DarkWalletImportView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DarkWalletImportView)
{
    ui->setupUi(this);

    ui->cancelButton->setStyleSheet(lightBackgroundStyleSheet());
    ui->importButton->setStyleSheet(lightBackgroundStyleSheet());
}


DarkWalletImportView::~DarkWalletImportView()
{
    delete ui;
}

void DarkWalletImportView::setFields(QString iAmount, QString iNotes)
{
    ui->totalLabel->setText(QString("%1 BTC").arg(iAmount));
    ui->noteLabel->setText(iNotes);
}

void DarkWalletImportView::on_closeWindowButton_pressed()
{
    this->hide();
    this->deleteLater();
}

void DarkWalletImportView::on_cancelButton_pressed()
{
    this->hide();
    this->deleteLater();

    emit addNotification(QDate::currentDate().toString(myDateFormat()), "Dark wallet import was canceled!");
}

void DarkWalletImportView::on_importButton_pressed()
{
    this->hide();
    this->deleteLater();

    emit addNotification(QDate::currentDate().toString(myDateFormat()), "Dark wallet import was successful!");
}
