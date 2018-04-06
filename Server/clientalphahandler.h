#ifndef CLIENTALPHAHANDLER_H
#define CLIENTALPHAHANDLER_H

#include "rpcmessage.h"
#include "clientconnection.h"

class ClientAlphaHandler
{
protected:
    static bool createUserAccount(ClientConnection * iConnection, RPCMessage &iMessage);

public:
    static bool handle(ClientConnection * iConnection, const QString iCommand, RPCMessage &iMessage);
};

#endif // CLIENTALPHAHANDLER_H