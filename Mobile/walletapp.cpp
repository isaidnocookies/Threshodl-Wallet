#include "walletapp.h"
#include "utils.h"

#include <QDebug>
#include <QDir>

WalletApp::WalletApp() {
    this->mTitle = "threeBX Magic Wallet";

    // An empty database file and its sql structure are available at folder `Local store` inside `Core` project
#ifdef Q_OS_MACOS
    qDebug() << "DB:" << QStringLiteral("%1%2%3").arg(QDir::currentPath()).arg(QDir::separator()).arg("wallets.store");
    SQLiteInterface* dataBase = new SQLiteInterface(QStringLiteral("%1%2%3").arg(QDir::currentPath()).arg(QDir::separator()).arg("../../../wallets.store"));
#else
    qDebug() << "DB:" << QStringLiteral("%1%2%3").arg(QDir::currentPath()).arg(QDir::separator()).arg("wallets.store");
    SQLiteInterface* dataBase = new SQLiteInterface(QStringLiteral("%1%2%3").arg(QDir::currentPath()).arg(QDir::separator()).arg("wallets.store"));
#endif

    qDebug() << "Creating a single wallet with no bills...";
    WalletEntity *wallet1 = new WalletEntity();
    wallet1->setOwner("guy1");
    wallet1->setCurrency("BTC");
    wallet1->setType(WalletType::WALLET_TYPE_LIGHT);

    if(dataBase->persist(wallet1)) {
        qDebug() << "Wallet1 id: " << wallet1->getId();
    }

    qDebug() << "Adding a bill after created...";
    BillEntity *bill = new BillEntity(wallet1);
    if(dataBase->persist(bill)) {
        qDebug() << "Bill Id: " << bill->getId();
    }

    qDebug() << "Creating a single wallet with two bills at once...";
    WalletEntity *wallet2 = new WalletEntity();
    wallet2->setOwner("guy2");
    wallet2->setCurrency("THDL");
    wallet2->setType(WalletType::WALLET_TYPE_LIGHT);

    BillEntity *bill1 = new BillEntity(wallet2);
    BillEntity *bill2 = new BillEntity(wallet2);
    wallet2->addBill(bill1);
    wallet2->addBill(bill2);
    if(dataBase->persist(wallet2)) {
        qDebug() << "Wallet2 Id: " << wallet2->getId() << ", Bills: " << bill1->getId() << " & " << bill2->getId();
    }

    qDebug() << "Listing all available wallets...";
    QList<Entity*> WalletList = dataBase->findAll(PersistenceType::PERSISTENCE_TYPE_WALLET);
    foreach(Entity* entity, WalletList) {
        WalletEntity* wallet = (WalletEntity*)entity;
        this->mWallets.append(wallet);
        qDebug() << "Id " << wallet->getId() << "Owner " << wallet->getOwner() << " amount " << wallet->getAmount() << wallet->getCurrency();
    }

    qDebug() << "Listing all available bills...";
    QList<Entity*> billList = dataBase->findAll(PersistenceType::PERSISTENCE_TYPE_BILL);
    foreach(Entity* entity, billList) {
        BillEntity* bill = (BillEntity*)entity;
        qDebug() << "Id:" << bill->getId() << " Owner:" << bill->getWallet()->getOwner() << " Wallet:" << bill->getWallet()->getId() << " Address:" << bill->getAddress() << " amount:" << bill->getAmount() << bill->getCurrency();
    }

    qDebug() << this->mWallets.length();
}

void WalletApp::setTitle(const QString &title) {
    this->mTitle = title;
    emit titleChanged();
}

QString WalletApp::getTitle() const {
    return this->mTitle;
}

QList<QObject*> WalletApp::listWallets() {
    QList<QObject*> list;
    foreach(WalletEntity* wallet, this->mWallets) {
        list.append(wallet);
    }
    return list;
}

void WalletApp::listener(const QString &param1, int param2) {
    qDebug() << param1 << " & " << param2;
}
