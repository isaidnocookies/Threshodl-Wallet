#include "walletapp.h"

#include <QDebug>

WalletApp::WalletApp()
{
    this->m_title = ",fgm bsd,gfhsldfjhglsdkf";
}

void WalletApp::setTitle(const QString &title) {
    qDebug() << "Changing title to: " << title;
    this->m_title = title;
}

QString WalletApp::getTitle() const {
    return this->m_title;
}

void WalletApp::listener(const QString &param1, int param2) {
    qDebug() << param1 << " & " << param2;
}
