#ifndef RPCMESSAGEREASSIGNMICROWALLETSREQUEST_H
#define RPCMESSAGEREASSIGNMICROWALLETSREQUEST_H

#include "rpcmessage.h"

class RPCMessageReassignMicroWalletsRequest : public RPCMessage
{
public:
    enum ReplyCode {
        UnknownFailure                  = 0x0,
        Success                         = 0x1,
        OneOrMoreWalletsUnauthorized    = 0x2,
        DestinationDoesNotExist         = 0x3,
        SourceDoesNotExist              = 0x4,
        InternalServerError1            = 0xA0,
    };

    typedef enum ReplyCode ReplyCode;

    RPCMessageReassignMicroWalletsRequest();
    RPCMessageReassignMicroWalletsRequest(const RPCMessage &iOther);
    RPCMessageReassignMicroWalletsRequest(const QString iMessage);

    static QString commandValue();
    static QString destinationKey();
    static QString microWalletIdsKey();
    static QString replyCodeKey();

    QString destination() const;
    QString walletIds() const;
    ReplyCode replyCode() const;
};

#endif // RPCMESSAGEREASSIGNMICROWALLETSREQUEST_H
