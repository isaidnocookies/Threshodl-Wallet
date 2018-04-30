#ifndef CLIENTHANDLERALPHA_H
#define CLIENTHANDLERALPHA_H

#include "rpcmessage.h"
#include "clientconnection.h"

class ClientHandlerAlpha
{
protected:
    static bool authenticateMessage(RPCMessage &iMessage);
    static bool createUserAccount(ClientConnection * iConnection, RPCMessage &iMessage);
    static bool createMicroWalletPackage(ClientConnection * iConnection, RPCMessage &iMessage); // This is incomplete, missing BTC transaction stuff
    static bool reassignMicroWallets(ClientConnection * iConnection, RPCMessage &iMessage);
    static bool checkOwnershipOfMicroWallets(ClientConnection * iConnection, RPCMessage &iMessage);
    static bool completeMicroWallets(ClientConnection * iConnection, RPCMessage &iMessage);

public:
    static bool handle(ClientConnection * iConnection, const QString iCommand, RPCMessage &iMessage);
};

#endif // CLIENTHANDLERALPHA_H