#include "darkmicrowalletview.h"
#include "ui_darkmicrowalletview.h"

#include "globalsandstyle.h"
#include "qstringmath.h"

#include <QtDebug>

DarkMicroWalletView::DarkMicroWalletView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DarkMicroWalletView)
{
    ui->setupUi(this);

    QScroller::grabGesture(ui->microWalletTableWidget, QScroller::LeftMouseButtonGesture);

    ui->refreshPushButton->setStyleSheet(darkBackgroundStyleSheet());
    ui->refreshPushButton->setVisible(false);

    ui->addressLabel->setTextFormat(Qt::RichText);
    ui->addressLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
    ui->addressLabel->setOpenExternalLinks(true);
}

DarkMicroWalletView::~DarkMicroWalletView()
{
    delete ui;
}

void DarkMicroWalletView::setMicroWallets(QList<BitcoinWallet> iMicros)
{
    int i = 0;
    QStringMath lTotal = QStringMath();

    for (auto entry : iMicros) {
        lTotal = lTotal + entry.value();
        ui->microWalletTableWidget->insertRow(i);
        ui->microWalletTableWidget->setItem(i, 0, new QTableWidgetItem(entry.value()));
        ui->microWalletTableWidget->setItem(i, 1, new QTableWidgetItem(QString(entry.address())));
        ui->microWalletTableWidget->item(i, 0)->setData(Qt::UserRole, entry.toData());

        i++;
    }

    ui->totalLabel->setText(QString("%1 BTC").arg(lTotal.toString()));
}

void DarkMicroWalletView::on_closeWindowButton_pressed()
{
    this->hide();
    this->deleteLater();
}

void DarkMicroWalletView::on_microWalletTableWidget_cellPressed(int row, int column)
{
    Q_UNUSED(column)

    BitcoinWallet lWallet (ui->microWalletTableWidget->item(row, 0)->data(Qt::UserRole).toByteArray());

    QString lAmount = ui->microWalletTableWidget->item(row, 0)->text();
    QString lAddress = ui->microWalletTableWidget->item(row, 1)->text();
    QString lWalletId = lWallet.walletId();

    ui->amountLabel->setText(QString("%1 BTC").arg(lAmount));
    ui->addressLabel->setText(QString("<a href=\"%1%2\" style=\"color: white;\">%2</a>").arg(BLOCKEXPLORER_ADDRESS_LINK_BASE).arg(lAddress));
    ui->walletIdLabel->setText(lWalletId);
}

void DarkMicroWalletView::on_refreshPushButton_pressed()
{

}
