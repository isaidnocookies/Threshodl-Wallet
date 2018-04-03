#include "walletapp.h"
#include "utils.h"

#include <QDebug>

WalletApp::WalletApp()
{
    this->mTitle = "threeBX Magic Wallet";
    Wallet* wallet1 = new Wallet(WalletMode::WALLET_MODE_LIGHT,"BTC","john");
    Wallet* wallet2 = new Wallet(WalletMode::WALLET_MODE_DARK,"BTC","Bob");
    wallet1->addBill(new Bill(wallet1));
    wallet1->addBill(new Bill(wallet1));
    this->mWallets.append(wallet1);
    this->mWallets.append(wallet2);

}

void WalletApp::setTitle(const QString &title) {
    this->mTitle = title;
    emit titleChanged();
}

QString WalletApp::getTitle() const {
    return this->mTitle;
}

QList<Wallet*> WalletApp::getWallets() {
    return this->mWallets;
}

QList<QObject*> WalletApp::listWallets() {
    QList<QObject*> list;
    foreach(Wallet* wallet, this->mWallets) {
        list.append(wallet);
    }
    return list;
}

int WalletApp::getCount() const {
    return this->mWallets.length();
}


void WalletApp::listener(const QString &param1, int param2) {
    qDebug() << param1 << " & " << param2;
}
