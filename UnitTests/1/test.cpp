#include "test.h"


Test::Test(RPCConnection * iConnection, QObject *iParent) : QObject(iParent)
{
    connect( iConnection, &RPCConnection::connected, this, &Test::rpcConnected );
}

void Test::rpcConnected()
{
    qDebug() << "Connected.";

}
