#include "darkmicrowalletview.h"
#include "ui_darkmicrowalletview.h"

#include "globalsandstyle.h"

#include <QtDebug>

DarkMicroWalletView::DarkMicroWalletView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DarkMicroWalletView)
{
    ui->setupUi(this);

    QScroller::grabGesture(ui->microWalletTableWidget, QScroller::LeftMouseButtonGesture);

    ui->refreshPushButton->setStyleSheet(darkBackgroundStyleSheet());
}

DarkMicroWalletView::~DarkMicroWalletView()
{
    delete ui;
}

void DarkMicroWalletView::setMicroWallets(QList<BitcoinWallet> iMicros)
{
    int i = 0;
    double lTotal = 0;

    for (auto entry : iMicros) {
        lTotal += entry.value().toDouble();
        ui->microWalletTableWidget->insertRow(i);
        ui->microWalletTableWidget->setItem(i, 0, new QTableWidgetItem(entry.value()));
        ui->microWalletTableWidget->setItem(i, 1, new QTableWidgetItem(QString(entry.address())));

        i++;
    }

    ui->totalLabel->setText(QString("%1 BTC").arg(QString::number(lTotal)));
}

void DarkMicroWalletView::on_closeWindowButton_pressed()
{
    this->hide();
    this->deleteLater();
}

void DarkMicroWalletView::on_microWalletTableWidget_cellPressed(int row, int column)
{
    Q_UNUSED(column)

    QString lAmount = ui->microWalletTableWidget->item(row, 0)->text();
    QString lAddress = ui->microWalletTableWidget->item(row, 1)->text();

    ui->amountLabel->setText(QString("%1 BTC").arg(lAmount));
    ui->addressLabel->setText(lAddress);
}

void DarkMicroWalletView::on_refreshPushButton_pressed()
{

}
