#ifndef RPCMESSAGEREASSIGNMICROWALLETSREPLY_H
#define RPCMESSAGEREASSIGNMICROWALLETSREPLY_H

#include "rpcmessage.h"

class RPCMessageReassignMicroWalletsReply : public RPCMessage
{
public:
    enum ReplyCode {
        UnknownFailure                  = 0x0,
        Success                         = 0x1,
        SourceDoesNotExist              = 0x2,
        DestinationDoesNotExist         = 0x3,
        OneOrMoreWalletsUnauthorized    = 0x4,
        OneOrMoreWalletsDoNotExist      = 0x5,
        InternalServerError1            = 0xA0,
        InternalServerError2            = 0xA1,
        InternalServerError3            = 0xA2
    };

    typedef enum ReplyCode ReplyCode;

    RPCMessageReassignMicroWalletsReply();
    RPCMessageReassignMicroWalletsReply(const RPCMessage &iOther);
    RPCMessageReassignMicroWalletsReply(const QString iMessage);

    static QString commandValue();
    static QString replyCodeKey();
    static QString transactionIdKey();

    ReplyCode replyCode() const;
    QString transactionId() const;      // Custom data passed back from the request untouched

    static QString create(
            const ReplyCode iReplyCode, const QString iTransactionId,
            const QString iUsername, const QByteArray iPrivateKey, RPCMessage::KeyEncoding iKeyEncoding = KeyEncoding::SHA512
            );
};

#endif // RPCMESSAGEREASSIGNMICROWALLETSREPLY_H
