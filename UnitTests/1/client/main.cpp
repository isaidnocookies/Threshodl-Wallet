#include "rpc.h"
#include "rpcconnection.h"

#include <QCoreApplication>

int main(int argc, char *argv[])
{
    QCoreApplication lApp(argc, argv);

    return lApp.exec();
}
