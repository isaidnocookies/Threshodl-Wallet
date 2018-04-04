#include <QCoreApplication>

#include "walletentity.h"
#include "billentity.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    WalletEntity *wallet = new WalletEntity(RepositoryType::REPOSITORY_TYPE_SQLITE);
    wallet->setOwner("someguy");
    wallet->setCurrency("BTC");
    wallet->setType(WalletType::WALLET_TYPE_LIGHT);

    BillEntity *bill = new BillEntity(wallet, RepositoryType::REPOSITORY_TYPE_SQLITE);
    bill->setAddress("skdfjho2347yp23p9523ghsdf5lk673l4562ggksdfgsdfgsdfg");
    bill->setPublicKey("94837523ot25go3r6w9erqtwefqwh4p72y34rqwerfqwrew56");
    bill->setPrivateKey("923487tlqfjreogqw47trqlagef468wtroqpfaqwffdgdyw45twgtsdge46ryr");
    wallet->addBill(bill);

    wallet->save();

    WalletEntity *entityReference = new WalletEntity();
    entityReference->loadAll();
    entityReference->load(23);

    return a.exec();
}
