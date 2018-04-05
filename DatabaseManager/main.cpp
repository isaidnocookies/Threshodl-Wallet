#include <QCoreApplication>

#include "walletentity.h"
#include "billentity.h"
#include "sqliteinterface.h"
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    SQLiteInterface* sqlite = new SQLiteInterface("wallets.store");

    qDebug() << "Creating a single wallet with no bills...";
    WalletEntity *wallet1 = new WalletEntity();
    wallet1->setOwner("guy1");
    wallet1->setCurrency("BTC");
    wallet1->setType(WalletType::WALLET_TYPE_LIGHT);

    if(sqlite->persist(wallet1)) {
        qDebug() << "Wallet1 id: " << wallet1->getId();
    }

    qDebug() << "Adding a bill after created...";
    BillEntity *bill = new BillEntity(wallet1);
    bill->setAddress("skdfjho2347yp23p9523ghsdf5lk673l4562ggksdfgsdfgsdfg");
    bill->setPublicKey("94837523ot25go3r6w9erqtwefqwh4p72y34rqwerfqwrew56");
    bill->setPrivateKey("923487tlqfjreogqw47trqlagef468wtroqpfaqwffdgdyw45");
    if(sqlite->persist(bill)) {
        qDebug() << "Bill Id: " << bill->getId();
    }

    qDebug() << "Creating a single wallet with two bills at once...";
    WalletEntity *wallet2 = new WalletEntity();
    wallet2->setOwner("guy2");
    wallet2->setCurrency("THDL");
    wallet2->setType(WalletType::WALLET_TYPE_LIGHT);

    BillEntity *bill1 = new BillEntity(wallet2);
    BillEntity *bill2 = new BillEntity(wallet2);
    bill1->setAddress("skdfjho2347yp23p9523ghsdf5lk673l4562ggksdfgsdfgsdfg");
    bill1->setPublicKey("94837523ot25go3r6w9erqtwefqwh4p72y34rqwerfqwrew56");
    bill1->setPrivateKey("923487tlqfjreogqw47trqlagef468wtroqpfaqwffdgdyw45");
    bill2->setAddress("sjdgfwi6u4trwqft9w4trfwo840w38rertyer6yeyrtsdfklgsdg");
    bill2->setPublicKey("923847vbterwfyteirfytwqv9386v0qbppsdse5bsengs569ys");
    bill2->setPrivateKey("lsdkfhgsoe7ty5sigfrhsdoirghsoedritsw73eo0tywpaerg");
    wallet2->addBill(bill1);
    wallet2->addBill(bill2);
    if(sqlite->persist(wallet2)) {
        qDebug() << "Wallet2 Id: " << wallet2->getId() << ", Bills: " << bill1->getId() << " & " << bill2->getId();
    }

    qDebug() << "Listing all available wallets...";
    QList<Entity*> WalletList = sqlite->findAll(PersistenceType::PERSISTENCE_TYPE_WALLET);
    foreach(Entity* entity, WalletList) {
        WalletEntity* wallet = (WalletEntity*)entity;
        qDebug() << "Id " << wallet->getId() << "Owner " << wallet->getOwner() << " amount " << wallet->getAmount() << wallet->getCurrency();
    }

    qDebug() << "Listing all available bills...";
    QList<Entity*> billList = sqlite->findAll(PersistenceType::PERSISTENCE_TYPE_BILL);
    foreach(Entity* entity, billList) {
        BillEntity* bill = (BillEntity*)entity;
        qDebug() << "Id:" << bill->getId() << " Owner:" << bill->getWallet()->getOwner() << " Wallet:" << bill->getWallet()->getId() << " Address:" << bill->getAddress() << " amount:" << bill->getAmount() << bill->getCurrency();
    }

    return a.exec();
}
