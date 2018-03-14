#include "rpc.h"
#include "rpcserver.h"

#include <QCoreApplication>

int main(int argc, char *argv[])
{
    QCoreApplication lApp(argc, argv);

    RPCServer * lServer = new RPCServer;

    return lApp.exec();
}
